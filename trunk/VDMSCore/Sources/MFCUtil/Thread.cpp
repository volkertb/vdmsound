#include "stdafx.h"

#include "Thread.h"

#include <eh.h>

CThread::CThread(
    IRunnable* target,
    LPCTSTR targetName,
    bool suspended,
    unsigned int stackSize)
  : m_hThread(NULL), m_pTarget(NULL)
{
  if (target != NULL) {
    Create(target, targetName, suspended, stackSize);
  }
}

CThread::~CThread(void) {
  if (m_hThread != NULL)
    Join(0);
}

bool CThread::Create(
    IRunnable* target,
    LPCTSTR targetName,
    bool suspended,
    unsigned int stackSize)
{
  ASSERT(m_hThread == NULL);
  ASSERT(target != NULL);

  m_pTarget      = target;
  m_szTargetName = targetName;
  m_dwThreadID   = 0;

  if ((m_hThread = (HANDLE)(::_beginthreadex(NULL, stackSize, ThreadProc, this, CREATE_SUSPENDED, (unsigned*)(&m_dwThreadID)))) == NULL)
    return false;

  if (suspended) {
    return true;
  } else {
    return Resume();
  }
}

bool CThread::Cancel(
    int timeout)
{
  ASSERT(m_hThread != NULL);

  PostMessage(WM_QUIT, NULL, NULL);
  return Join(timeout);
}

bool CThread::Join(
    int timeout)
{
  ASSERT(m_hThread != NULL);

  if (::WaitForSingleObject(m_hThread, timeout) == WAIT_TIMEOUT)
    return false;

  CloseHandle(m_hThread);
  m_hThread = NULL;
  return true;
}

bool CThread::Suspend(void) {
  ASSERT(m_hThread != NULL);
  return (::SuspendThread(m_hThread) != (DWORD)(-1));
}

bool CThread::Resume(void) {
  ASSERT(m_hThread != NULL);
  return (::ResumeThread(m_hThread) != (DWORD)(-1));
}

int CThread::GetPriority(void) {
  ASSERT(m_hThread != NULL);
  return ::GetThreadPriority(m_hThread);
}

bool CThread::SetPriority(
    int priority)
{
  ASSERT(m_hThread != NULL);
  return (::SetThreadPriority(m_hThread, priority) != FALSE);
}

IRunnable* CThread::GetTarget(void) {
  ASSERT(m_hThread != NULL);
  return m_pTarget;
}

LPCTSTR CThread::GetTargetName(void) {
  return m_szTargetName;
}

DWORD CThread::GetThreadID(void) {
  ASSERT(m_hThread != NULL);
  return m_dwThreadID;
}

HANDLE CThread::GetThreadHandle(void) {
  return m_hThread;
}

bool CThread::PostMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
  ASSERT(m_hThread != NULL);
  return (::PostThreadMessage(GetThreadID(), message, wParam, lParam) != FALSE);
}

bool CThread::GetMessage(
    MSG* message,
    bool isBlocking)
{
  ASSERT(m_hThread != NULL);

  if (isBlocking) {
    return (::GetMessage(message, NULL, 0, 0) != -1);
  } else {
    return (::PeekMessage(message, NULL, 0, 0, PM_REMOVE | PM_NOYIELD) != 0);
  }
}

unsigned int WINAPI CThread::ThreadProc(
    LPVOID lpParam)
{
  CThread* pThis = (CThread*)(lpParam);
  IRunnable* pRunnable = ((pThis != NULL) ? pThis->m_pTarget : NULL);

  ASSERT(pRunnable != NULL);

  _set_se_translator(ThreadSETranslator);

  unsigned int status;

  do {
    try {
      status = pRunnable->Run(*pThis);
      break;
    } catch (CException* ce) {
      TCHAR buf[1024];
      if ((!ce) || (!ce->GetErrorMessage(buf, sizeof(buf)/sizeof(buf[0])))) {
        _tcscpy(buf, _T("(no description available)"));
      }

      if (MessageBox(Format(_T("An exception occured in thread %d (%s).\n\n%s\n\nDo you want to continue?"), (int)(pThis->GetThreadID()), pThis->GetTargetName(), buf),
                            _T("Error"),
                     MB_YESNO|MB_DEFBUTTON1, MB_ICONERROR) == IDNO)
      {
        throw;
      }
    } catch (...) {
      if (MessageBox(Format(_T("An unknown exception occured in thread %d (%s).\n\nDo you want to continue?"), (int)(pThis->GetThreadID()), (int)(pThis->GetTargetName())),
                            _T("Error"),
                     MB_YESNO|MB_DEFBUTTON1, MB_ICONERROR) == IDNO)
      {
        throw;
      }
    }
  } while (true);

  _endthreadex(status);

  return status;
}

void CThread::ThreadSETranslator(
  unsigned int exCode,
  EXCEPTION_POINTERS* pExInfo)
{
  throw new CWin32StdException(TRUE, pExInfo ? pExInfo->ExceptionRecord : NULL);
}
