#include "stdafx.h"

#include "MPU401CtlFSM.h"
#include "MPU401CtlConst.h"

CMPU401CtlFSM::CMPU401CtlFSM(IMPU401HWEmulationLayer* _hwemu)
  : mode(M_INTELLIGENT), hwemu(_hwemu), inBuf(_hwemu), outBuf(_hwemu)
{
  ASSERT(_hwemu != NULL);
}

CMPU401CtlFSM::~CMPU401CtlFSM(void)
{
}


/////////////////////////////////////////////////////////////////////////////


//
// Called when a command byte is output to the MPU-401's command/status port
//
void CMPU401CtlFSM::putCommand(
    char command)
{
  switch (command) {
    // REQUEST to switch to UART mode
    case CMD_UART_MODE:
      mode = M_UART;                // switched to UART mode
      inBuf.putByte(0xfe);          // acknowledge command
      return;

    // REQUEST to reset (and switch to INTELLIGENT mode)
    case CMD_RESET:
      mode = M_INTELLIGENT;         // switched to intelligent mode
      inBuf.reset();                // flush the input buffer
      outBuf.reset();               // flush the output buffer
      inBuf.putByte(0xfe);          // acknowledge command
      return;

    // Illegal or unsupported command
    default:
      inBuf.putByte(0xfe);          // acknowledge command
      return;
  }
}

//
// Called when the status is read from the MPU-401's command/status port
//
char CMPU401CtlFSM::getStatus(void) {
  return (char)MK_MPU_STATUS(!inBuf.isEmpty(), !outBuf.isFull());
}

//
// Called when a data byte is output to the MPU-401's data port
//
void CMPU401CtlFSM::putData(
    char data)
{
  outBuf.putByte(data);             // Transmit the data on to the MIDI device
}

//
// Called when a data byte is read from the MPU-401's data port
//
char CMPU401CtlFSM::getData(void) {
  BYTE data = 0xff;
  inBuf.getByte(&data);             // Input (buffered) data from the MIDI device
  return data;
}
