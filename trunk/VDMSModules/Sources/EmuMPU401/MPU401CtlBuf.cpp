#include "stdafx.h"

#include "MPU401CtlBuf.h"

#include "MIDIConst.h"

/////////////////////////////////////////////////////////////////////////////
//
// CMIDIInputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIInputBuffer::CMIDIInputBuffer(IMPU401HWEmulationLayer* hwemu)
  : m_isEmptyBuf(true), m_IRQPending(false), m_hwemu(hwemu)
{
  ASSERT(m_hwemu != NULL);
}

CMIDIInputBuffer::~CMIDIInputBuffer(void)
{
}

void CMIDIInputBuffer::putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length) {
  _QUEUEPUT_PROLOGUE();
  m_buf.push(status);
  if (length >= 1) m_buf.push(data1);
  if (length >= 2) m_buf.push(data2);
  _QUEUEPUT_EPILOGUE();
}

void CMIDIInputBuffer::putSysEx(const unsigned char * data, long length) {
  _QUEUEPUT_PROLOGUE();
  m_buf.push(MIDI_EVENT_SYSTEM_SYSEX);
  for (int i = 0; i < length; i++) m_buf.push(data[i]);
  m_buf.push(MIDI_EVENT_SYSTEM_EOX);
  _QUEUEPUT_EPILOGUE();
}

void CMIDIInputBuffer::putRealTime(unsigned char data) {
  _QUEUEPUT_PROLOGUE();
  m_buf.push(data);
  _QUEUEPUT_EPILOGUE();
}

/////////////////////////////////////////////////////////////////////////////

void CMIDIInputBuffer::reset(void) {
  m_mutex.lock();

  while (!m_buf.empty())
    m_buf.pop();                    // flush the buffer

  m_isEmptyBuf = true;              // mark as empty
  m_IRQPending = false;             // clear interrupt
  m_mutex.unlock();
}

void CMIDIInputBuffer::putByte(unsigned char data) {
  _QUEUEPUT_PROLOGUE();
  m_buf.push(data);
  _QUEUEPUT_EPILOGUE();
}

bool CMIDIInputBuffer::getByte(unsigned char* data) {
  if (m_buf.empty())
    return false;                   // no data to retrieve

  _QUEUEGET_PROLOGUE();
  *data = m_buf.front();
  m_buf.pop();
  _QUEUEGET_EPILOGUE();
  return true;
}



/////////////////////////////////////////////////////////////////////////////
//
// CMIDIOutputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIOutputBuffer::CMIDIOutputBuffer(IMPU401HWEmulationLayer* hwemu)
  : m_currentEvent(0x00), m_hwemu(hwemu)
{
  ASSERT(m_hwemu != NULL);
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
      if (m_buf.size() < 4) {
        std::ostringstream oss;
        oss << std::setbase(10) << "SysEx too short (" << m_buf.size() << " bytes); ended by event 0x" << std::setbase(16) << (data & 0xff) << ".";
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

    if (!m_buf.empty()) {
      std::ostringstream oss;
      oss << std::setbase(16) << "Non-realtime event (0x" << (data & 0xff) << ") occured during another event (0x" << (m_currentEvent & 0xff) << ", " << std::setbase(10) << m_buf.size() << " bytes to date)";
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
