#ifndef __MPU401CTLFSM_H_
#define __MPU401CTLFSM_H_

#include "MPU401CtlBuf.h"

//
// This class implements the finite state machine logic behind the MPU-401
//
class CMPU401CtlFSM
  : IMPU401MIDIConsumer
{
  protected:
    enum mode_t {
      M_INTELLIGENT,
      M_UART
    };

  public:
    CMPU401CtlFSM(IMPU401HWEmulationLayer* _hwemu);
    ~CMPU401CtlFSM(void);

  public:
    inline void putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length)
      { inBuf.putEvent(status, data1, data2, length); };
    inline void putSysEx(const unsigned char * data, long length)
      { inBuf.putSysEx(data, length); };
    inline void putRealTime(unsigned char data)
      { inBuf.putRealTime(data); };

  public:
    void putCommand(char data);
    char getStatus(void);
    void putData(char data);
    char getData(void);

  protected:
    mode_t mode;

  protected:
    IMPU401HWEmulationLayer* hwemu;
    CMIDIInputBuffer inBuf;
    CMIDIOutputBuffer outBuf;
};

#endif __MPU401CTLFSM_H_
