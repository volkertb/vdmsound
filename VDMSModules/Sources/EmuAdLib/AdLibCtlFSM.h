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
    virtual void resetOPL(void) = 0;
    virtual void setOPLReg(int chipID, int regSet, int regIdx, int value) = 0;
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
    enum type_t { TYPE_OPL2, TYPE_OPL3 };

  public:
    CAdLibCtlFSM(IAdLibHWEmulationLayer* hwemu, int chipID);
    ~CAdLibCtlFSM(void);

  public:
    void setType(type_t type);
    type_t getType(void);

    void reset(void);

    char read(int port);
    void write(int port, char data);

  protected:
    void setRegister(int address, char data);
    void clearStatus(bool T1 = true, bool T2 = true);
    void clearTimers(OPLTime_t tNow, bool T1 = true, bool T2 = true);
    char updateTimers(OPLTime_t tNow);

  protected:
    type_t      m_type;                 // OPL type (OPL2, OPL3, etc.)
    char        m_status;               // OPL status byte (flags)
    int         m_chipID;               // unique chip identifier (passed to callbacks)
    int         m_regIdx;               // the currently-selected register index
    bool        m_OPL2_compat;          // OPL2 compatibility mode
    OPLTimer_t  m_timer1, m_timer2;     // the timer values

  protected:
    IAdLibHWEmulationLayer* m_hwemu;
};

#endif //__ADLIBCTLFSM_H_
