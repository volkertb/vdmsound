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
    CMPU401CtlFSM(IMPU401HWEmulationLayer* hwemu);
    ~CMPU401CtlFSM(void);

  public:
    inline void putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length)
      { m_inBuf.putEvent(status, data1, data2, length); }
    inline void putSysEx(const unsigned char * data, long length)
      { m_inBuf.putSysEx(data, length); }
    inline void putRealTime(unsigned char data)
      { m_inBuf.putRealTime(data); }

  public:
    void reset(void);
    void putCommand(char data);
    char getStatus(void);
    void putData(char data);
    char getData(void);

  protected:
    mode_t m_mode;

  protected:
    IMPU401HWEmulationLayer* m_hwemu;
    CMIDIInputBuffer m_inBuf;
    CMIDIOutputBuffer m_outBuf;
};

#endif __MPU401CTLFSM_H_
