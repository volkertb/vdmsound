#ifndef __THREAD_H__
#define __THREAD_H__

class CThread;

class IRunnable {
  public:
    virtual unsigned int Run(CThread& thread) = 0;
};

//
//
//
class CThread {
  public:
    CThread(IRunnable* target = NULL, bool suspended = false, unsigned int stackSize = 0);
    ~CThread(void);

  public:
    bool Create(IRunnable* target, bool suspended = false, unsigned int stackSize = 0);
    bool Cancel(int timeout = 125);
    bool Join(int timeout = INFINITE);
    bool Suspend(void);
    bool Resume(void);

  public:
    bool PostMessage(UINT message, WPARAM wParam, LPARAM lParam);
    bool GetMessage(MSG* message, bool isBlocking = true);
    int GetPriority(void);
    bool SetPriority(int priority);
    IRunnable* GetTarget(void);
    DWORD GetThreadID(void);
    HANDLE GetThreadHandle(void);

  protected:
    static unsigned int WINAPI ThreadProc(LPVOID lpParam);

  protected:
    HANDLE m_hThread;
    DWORD m_dwThreadID;
    IRunnable* m_pTarget;
};

#endif // __THREAD_H__
