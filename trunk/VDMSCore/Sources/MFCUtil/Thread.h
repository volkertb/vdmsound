#ifndef __THREAD_H__
#define __THREAD_H__

#include "MFCUtil.h"

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
    CThread(IRunnable* target = NULL, LPCTSTR targetName = NULL, bool suspended = false, unsigned int stackSize = 0);
    ~CThread(void);

  public:
    bool Create(IRunnable* target, LPCTSTR targetName = NULL, bool suspended = false, unsigned int stackSize = 0);
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
    LPCTSTR GetTargetName(void);
    DWORD GetThreadID(void);
    HANDLE GetThreadHandle(void);

  protected:
    //
    // Win32 standard exception wrapper
    //
    class CWin32StdException : public CException {
      public:
        CWin32StdException(BOOL b_AutoDelete, EXCEPTION_RECORD* pExRecord) 
          : CException(b_AutoDelete)
        {
          if (pExRecord) {
            m_exRecord = *pExRecord;
          } else {
            memset(&m_exRecord, 0, sizeof(m_exRecord));
          }
        }

        virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL) {
          try {
            // Attempt to obtain a description for the exception
            CString exDescription = GetExceptionText(m_exRecord.ExceptionCode);

            if (exDescription.IsEmpty())
              exDescription = _T("(no exception description is available)");

            // Enumerate the exception's parameters (if any)
            CString exParams;
            for (DWORD exParamIdx = 0; exParamIdx < m_exRecord.NumberParameters; exParamIdx++)
              exParams += FormatMessage(_T("0x%1!08x! "), false, m_exRecord.ExceptionInformation[exParamIdx]);

            if (exParams.IsEmpty())
              exParams = _T("(no exception parameters are available)");

            DWORD args[] = { m_exRecord.ExceptionCode, (DWORD)m_exRecord.ExceptionAddress, (DWORD)(LPCTSTR)exDescription, (DWORD)(LPCTSTR)exParams };

            return (FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, _T("0x%1!08x! - %3%nFault location: 0x%2!08x!%nParameters: %4"), 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpszError, nMaxError, (char**)args) != 0);
          } catch (...) {
            return FALSE;
          }
        }

      protected:
        EXCEPTION_RECORD m_exRecord;
    };

  protected:
    static unsigned int WINAPI ThreadProc(LPVOID lpParam);
    static void ThreadSETranslator(unsigned int exCode, EXCEPTION_POINTERS* pExInfo);

  protected:
    HANDLE m_hThread;
    DWORD m_dwThreadID;
    IRunnable* m_pTarget;
    LPCTSTR m_szTargetName;
};

#endif // __THREAD_H__
