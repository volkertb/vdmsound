#include "stdafx.h"

#include "MPU401CtlBuf.h"

#include "MIDIConst.h"

/////////////////////////////////////////////////////////////////////////////
//
// CMIDIInputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIInputBuffer::CMIDIInputBuffer(IMPU401HWEmulationLayer* hwemu)
  : m_buf_len(0), m_buf_pos(0), m_IRQPending(false), m_hwemu(hwemu)
{
  _ASSERTE(m_hwemu != NULL);
}

CMIDIInputBuffer::~CMIDIInputBuffer(void)
{
}

void CMIDIInputBuffer::putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length) {
  // TODO: check if "recording" first
  putByte(status);
  if (length >= 1) putByte(data1);
  if (length >= 2) putByte(data2);
}

void CMIDIInputBuffer::putSysEx(const unsigned char * data, long length) {
  // TODO: check if "recording" first
  putByte(MIDI_EVENT_SYSTEM_SYSEX);
  for (int i = 0; i < length; i++) putByte(data[i]);
  putByte(MIDI_EVENT_SYSTEM_EOX);
}

void CMIDIInputBuffer::putRealTime(unsigned char data) {
  // TODO: check if "recording" first
  putByte(data);
}

/////////////////////////////////////////////////////////////////////////////

void CMIDIInputBuffer::reset(void) {
  m_mutex.Lock();

  m_buf_len = 0;                    // flush the buffer

  // do *not* clear any pending interrupts

  m_mutex.Unlock();
}

void CMIDIInputBuffer::putByte(unsigned char data) {
  m_mutex.Lock();

  if (m_buf_len < MIDI_BUF_LEN) {
    m_buf[(m_buf_pos + m_buf_len) % MIDI_BUF_LEN] = data;
    m_buf_len = (m_buf_len + 1) % (MIDI_BUF_LEN + 1);

    if (!m_IRQPending) {
      m_hwemu->generateInterrupt();
      m_IRQPending = true;
    }
  } else {
    std::ostringstream oss;
    oss << std::setbase(10) << "MIDI-in buffer is full (" << m_buf_len << "  bytes), discarding data.";
    m_hwemu->logError(oss.str().c_str());
  }

  m_mutex.Unlock();
}

bool CMIDIInputBuffer::getByte(unsigned char* data) {
  bool retVal = true;

  m_mutex.Lock();

  m_IRQPending = false;           // reading this byte acknowledges the last IRQ (if any)
  *data = m_buf[m_buf_pos];       // read the data byte at the head of the queue

  if (m_buf_len > 1) {            // more than one data byte was waiting in the queue
    m_buf_len--;
    m_buf_pos = (m_buf_pos + 1) % MIDI_BUF_LEN;

    m_hwemu->generateInterrupt(); // signal that additional data bytes are waiting in the queue
    m_IRQPending = true;
  } else if (m_buf_len > 0) {     // exactly one data byte was waiting in the queue
    m_buf_len = 0;                // no need to advance queue head now; keep it where it is so that "undeflow" reads always return the last value.
  } else {                        // the queue was empty
    retVal = false;
  }

  m_mutex.Unlock();

  return retVal;
}



/////////////////////////////////////////////////////////////////////////////
//
// CMIDIOutputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIOutputBuffer::CMIDIOutputBuffer(IMPU401HWEmulationLayer* hwemu)
  : m_currentEvent(0x00), m_hwemu(hwemu)
{
  _ASSERTE(m_hwemu != NULL);
}

CMIDIOutputBuffer::~CMIDIOutputBuffer(void)
{
}

void CMIDIOutputBuffer::reset(void) {
  m_buf.clear();                    // flush the buffer
  m_currentEvent = 0x00;            // clear the running-status
}

void CMIDIOutputBuffer::putByte(
    unsigned char data)
{
  if (IS_REALTIME_EVENT(data)) {
    m_hwemu->putRealTime(data);     // let through the realtime event
    return;
  }

  if (IS_MIDI_EVENT(data)) {
    if (m_currentEvent == MIDI_EVENT_SYSTEM_SYSEX) {
      if (m_buf.size() + 1 < MIDI_evt_len[MIDI_EVENT_SYSTEM_SYSEX]) {
        std::ostringstream oss;
        oss << std::setbase(10) << "SysEx too short (" << m_buf.size() << " bytes, ended by event 0x" << std::setbase(16) << (data & 0xff) << "), flushing";
        m_hwemu->logError(oss.str().c_str());
        m_buf.clear();
      } else {
        m_hwemu->putSysEx(m_buf.begin(), m_buf.size()); // let through the system exclusive event
        m_buf.clear();

        if (data == MIDI_EVENT_SYSTEM_EOX) {
          m_currentEvent = 0x00;    // clear running status; don't want next event to find currentEvent==SYSEX with an empty queue ("SysEx too short" error)
          return;
        }
      }
    }

    if (MIDI_evt_len[data & 0xff] < 1) {
      std::ostringstream oss;
      oss << std::setbase(16) << "Undefined event (0x" << (data & 0xff) << ") occured during another event (0x" << (m_currentEvent & 0xff) << ", " << std::setbase(10) << m_buf.size() << " bytes to date), ignoring";
      m_hwemu->logError(oss.str().c_str());
      return;
    }

    if (data == MIDI_EVENT_SYSTEM_EOX) {
      std::ostringstream oss;
      oss << std::setbase(16) << "EOX event (0x" << (data & 0xff) << ") occured outside of SysEx event (0x" << (m_currentEvent & 0xff) << ", " << std::setbase(10) << m_buf.size() << " bytes to date), ignoring";
      m_hwemu->logError(oss.str().c_str());
      return;
    }

    if (!m_buf.empty()) {
      std::ostringstream oss;
      oss << std::setbase(16) << "Non-realtime event (0x" << (data & 0xff) << ") occured during another event (0x" << (m_currentEvent & 0xff) << ", " << std::setbase(10) << m_buf.size() << " bytes to date), flushing";
      m_hwemu->logError(oss.str().c_str());
      m_buf.clear();
    }

    m_currentEvent = data;          // set running status
  } else {
    if (isFull()) {
      m_hwemu->logError("MIDI-out buffer is full");
    } else {
      m_buf.push_back(data);
    }
  }

  if ((m_currentEvent != MIDI_EVENT_SYSTEM_SYSEX) &&
      (m_buf.size() + 1 == MIDI_evt_len[m_currentEvent & 0xff]))
  {
    m_hwemu->putEvent(m_currentEvent, m_buf.size() > 0 ? m_buf[0] : 0x00, m_buf.size() > 1 ? m_buf[1] : 0x00, m_buf.size());  // let through the event
    m_buf.clear();
    return;
  }
}
