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
  int base = 0;
  std::ostringstream oss;

  switch (command & 0xff) {
    // REQUEST to switch to UART mode
    case CMD_UART_MODE:
      m_hwemu->logInformation("MPU-401 Switched to UART mode");
      m_mode = M_UART;              // switched to UART mode
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      return;

    // REQUEST to reset (and switch to INTELLIGENT mode)
    case CMD_RESET:
      m_hwemu->logInformation("MPU-401 reset");
      m_mode = M_INTELLIGENT;       // switched to intelligent mode
      reset();                      // flush buffers
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      return;

#if 0 /* doesn't fix Legend games, and wouldn't want to break other games (yet) */

    case CMD_REQUEST_VERSION:
      m_hwemu->logInformation("MPU-401 request MIDI version");
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      m_inBuf.putByte(0x15);        // version (bits 7-4 = major, bits 3-0 = minor)
      return;

    case CMD_REQUEST_REVISION:
      m_hwemu->logInformation("MPU-401 request revision");
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      m_inBuf.putByte(0x01);        // revision number
      return;

    case CMD_INT_CLOCK:
      m_hwemu->logInformation("MPU-401 internal clock select");
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      return;

    case CMD_TIMEBASE_48:
    case CMD_TIMEBASE_72:
    case CMD_TIMEBASE_96:
    case CMD_TIMEBASE_120:
    case CMD_TIMEBASE_144:
    case CMD_TIMEBASE_168:
    case CMD_TIMEBASE_192:
      base = ((command & 0xff) - CMD_TIMEBASE_48) * 24 + 48;
      oss << "MPU-401 timebase select (" << base << " \?\?\?)";
      m_hwemu->logInformation(oss.str().c_str());
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
      m_hwemu->setTimerPeriod(1000 / base);
      return;

#endif

    // Illegal or unsupported command
    default:
      oss << std::setbase(16) << "Illegal or unsupported MPU-401 command (0x" << (command & 0xff) << ", " << (m_mode == M_UART ? "UART" : m_mode == M_INTELLIGENT ? "intelligent" : "<unknown>") << " mode), faking acknowledge";
      m_hwemu->logError(oss.str().c_str());
      m_inBuf.putByte(MSG_CMD_ACK); // acknowledge command
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

//
// Called when the MPU timer expires
//
void CMPU401CtlFSM::timerExpired(void) {
#if 0 /* doesn't fix Legend games, and wouldn't want to break other games (yet) */
  m_inBuf.putByte(CMD_TIMING_OVERFLOW);
#endif
}
