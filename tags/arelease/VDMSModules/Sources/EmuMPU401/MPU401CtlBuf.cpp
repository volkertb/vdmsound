#include "stdafx.h"

#include "MPU401CtlBuf.h"

#include "MIDIConst.h"

/////////////////////////////////////////////////////////////////////////////
//
// CMIDIInputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIInputBuffer::CMIDIInputBuffer(IMPU401HWEmulationLayer* _hwemu)
  : isEmptyBuf(true), IRQPending(false), hwemu(_hwemu)
{
  ASSERT(hwemu != NULL);
}

CMIDIInputBuffer::~CMIDIInputBuffer(void)
{
}

void CMIDIInputBuffer::putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length) {
  _QUEUEPUT_PROLOGUE();
  buf.push(status);
  if (length >= 1) buf.push(data1);
  if (length >= 2) buf.push(data2);
  _QUEUEPUT_EPILOGUE();
}

void CMIDIInputBuffer::putSysEx(const unsigned char * data, long length) {
  _QUEUEPUT_PROLOGUE();
  buf.push(MIDI_EVENT_SYSTEM_SYSEX);
  for (int i = 0; i < length; i++) buf.push(data[i]);
  buf.push(MIDI_EVENT_SYSTEM_EOX);
  _QUEUEPUT_EPILOGUE();
}

void CMIDIInputBuffer::putRealTime(unsigned char data) {
  _QUEUEPUT_PROLOGUE();
  buf.push(data);
  _QUEUEPUT_EPILOGUE();
}

/////////////////////////////////////////////////////////////////////////////

void CMIDIInputBuffer::reset(void) {
  mutex.lock();
  while (!buf.empty()) buf.pop();   // flush the buffer
  isEmptyBuf = true;                // mark as empty
  IRQPending = false;               // clear interrupt
  mutex.unlock();
}

void CMIDIInputBuffer::putByte(unsigned char data) {
  _QUEUEPUT_PROLOGUE();
  buf.push(data);
  _QUEUEPUT_EPILOGUE();
}

bool CMIDIInputBuffer::getByte(unsigned char* data) {
  if (buf.empty())
    return false;                   // no data to retrieve

  _QUEUEGET_PROLOGUE();
  *data = buf.front();
  buf.pop();
  _QUEUEGET_EPILOGUE();
  return true;
}



/////////////////////////////////////////////////////////////////////////////
//
// CMIDIOutputBuffer
//
/////////////////////////////////////////////////////////////////////////////

CMIDIOutputBuffer::CMIDIOutputBuffer(IMPU401HWEmulationLayer* _hwemu)
  : currentEvent(0x00), hwemu(_hwemu)
{
  ASSERT(hwemu != NULL);
}

CMIDIOutputBuffer::~CMIDIOutputBuffer(void)
{
}

void CMIDIOutputBuffer::reset(void) {
  buf.clear();                      // flush the buffer
  currentEvent = 0x00;              // clear the running-status
}

void CMIDIOutputBuffer::putByte(
    unsigned char data)
{
  if (IS_REALTIME_EVENT(data)) {
    hwemu->putRealTime(data);       // let through the realtime event
    return;
  }

  if (IS_MIDI_EVENT(data)) {
    if (currentEvent == MIDI_EVENT_SYSTEM_SYSEX) {
      if (buf.size() < 4) {
        std::ostringstream oss;
        oss << std::setbase(10) << "SysEx too short (" << buf.size() << " bytes); ended by event 0x" << std::setbase(16) << (data & 0xff) << ".";
        hwemu->logMessage(IMPU401HWEmulationLayer::MSG_ERROR, oss.str().c_str());
        buf.clear();
      } else {
        hwemu->putSysEx(buf.begin(), buf.size());   // let through the system exclusive event
        buf.clear();

        if (data == MIDI_EVENT_SYSTEM_EOX) {
          currentEvent = 0x00;      // clear running status; don't want next event to find currentEvent==SYSEX with an empty queue ("SysEx too short" error)
          return;
        }
      }
    }

    if (!buf.empty()) {
      std::ostringstream oss;
      oss << std::setbase(16) << "Non-realtime event (0x" << (data & 0xff) << ") occured during another event (0x" << (currentEvent & 0xff) << ", " << std::setbase(10) << buf.size() << " bytes to date)";
      hwemu->logMessage(IMPU401HWEmulationLayer::MSG_ERROR, oss.str().c_str());
      buf.clear();
    }
    currentEvent = data;            // set running status
  } else {
    if (isFull()) {
      hwemu->logMessage(IMPU401HWEmulationLayer::MSG_ERROR, "MIDI-out buffer is full");
    } else {
      buf.push_back(data);
    }
  }

  if ((currentEvent != MIDI_EVENT_SYSTEM_SYSEX) &&
      (buf.size() + 1 == MIDI_evt_len[currentEvent & 0xff]))
  {
    hwemu->putEvent(currentEvent, buf.size() > 0 ? buf[0] : 0x00, buf.size() > 1 ? buf[1] : 0x00, buf.size());   // let through the event
    buf.clear();
    return;
  }
}
