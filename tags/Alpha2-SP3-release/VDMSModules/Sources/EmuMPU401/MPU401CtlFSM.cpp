#include "stdafx.h"

#include "MPU401CtlFSM.h"
#include "MPU401CtlConst.h"

CMPU401CtlFSM::CMPU401CtlFSM(IMPU401HWEmulationLayer* hwemu)
  : m_mode(M_INTELLIGENT), m_hwemu(hwemu), m_inBuf(hwemu), m_outBuf(hwemu)
{
  _ASSERTE(m_hwemu != NULL);
}

CMPU401CtlFSM::~CMPU401CtlFSM(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Resets the MPU-401
//
void CMPU401CtlFSM::reset(void) {
  m_inBuf.reset();                  // flush the input buffer
  m_outBuf.reset();                 // flush the output buffer
}

//
// Called when a command byte is output to the MPU-401's command/status port
//
void CMPU401CtlFSM::putCommand(
    char command)
{
  switch (command & 0xff) {
    // REQUEST to switch to UART mode
    case CMD_UART_MODE:
      m_hwemu->logInformation("MPU-401 Switched to UART mode");
      m_mode = M_UART;              // switched to UART mode
      m_inBuf.putByte(0xfe);        // acknowledge command
      return;

    // REQUEST to reset (and switch to INTELLIGENT mode)
    case CMD_RESET:
      m_hwemu->logInformation("MPU-401 reset");
      m_mode = M_INTELLIGENT;       // switched to intelligent mode
      reset();                      // flush buffers
      m_inBuf.putByte(0xfe);        // acknowledge command
      return;

    // Illegal or unsupported command
    default:
      std::ostringstream oss;
      oss << std::setbase(16) << "Illegal or unsupported MPU-401 command (0x" << (command & 0xff) << ", " << (m_mode == M_UART ? "UART" : m_mode == M_INTELLIGENT ? "intelligent" : "<unknown>") << " mode), faking acknowledge";
      m_hwemu->logError(oss.str().c_str());
      m_inBuf.putByte(0xfe);        // acknowledge command
      return;
  }
}

//
// Called when the status is read from the MPU-401's command/status port
//
char CMPU401CtlFSM::getStatus(void) {
  return (char)MK_MPU_STATUS(!m_inBuf.isEmpty(), !m_outBuf.isFull());
}

//
// Called when a data byte is output to the MPU-401's data port
//
void CMPU401CtlFSM::putData(
    char data)
{
  m_outBuf.putByte(data);           // Transmit the data on to the MIDI device
}

//
// Called when a data byte is read from the MPU-401's data port
//
char CMPU401CtlFSM::getData(void) {
  BYTE data = MSG_CMD_ACK;

  if (!m_inBuf.getByte(&data)) {    // Input (buffered) data from the MIDI device
    m_hwemu->logError("Attempted to read from empty MPU-401 inbound FIFO");
  }

  return data;
}
