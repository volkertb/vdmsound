#ifndef __ADLIBCTLFSM_H_
#define __ADLIBCTLFSM_H_

//
// Define a type to store time intervals (in microseconds)
//
typedef __int64 OPLTime_t;



//
// This interface isolates the OPL's timer state management (taken
//  care of in CAdLibCtlFSM) and the FM synthesis part (taken care
//  of externally, and controlled via this interface).  It also gives
//  access to high-resolution time information
//
class IAdLibHWEmulationLayer {
  public:
    virtual void setOPLReg(int address, int value) = 0;
    virtual OPLTime_t getTimeMicros(void) = 0;
    virtual void logError(const char* message) = 0;
    virtual void logWarning(const char* message) = 0;
    virtual void logInformation(const char* message) = 0;
};



//
// This class implements the finite state machine logic behind the OPL
//  timers
//
class CAdLibCtlFSM {
  protected:
    struct OPLTimer_t {
      bool isEnabled;
      bool isMasked;
      OPLTime_t period;
      OPLTime_t expiry;
    };

  public:
    CAdLibCtlFSM(IAdLibHWEmulationLayer* hwemu);
    ~CAdLibCtlFSM(void);

  public:
    void reset(void);
    char getStatus(void);
    void setAddress(char data);
    void putData(char data);

  protected:
    void clearStatus(bool T1 = true, bool T2 = true);
    void clearTimers(OPLTime_t tNow, bool T1 = true, bool T2 = true);
    char updateTimers(OPLTime_t tNow);

  protected:
    char m_status;
    int m_regIdx;                       // the currently-selected register
    OPLTimer_t m_timer1, m_timer2;      // the timer values

  protected:
    IAdLibHWEmulationLayer* m_hwemu;
};

#endif //__ADLIBCTLFSM_H_
