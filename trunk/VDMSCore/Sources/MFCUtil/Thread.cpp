#include "stdafx.h"

#include "Thread.h"

CThread::CThread(
    IRunnable* target,
    bool suspended,
    unsigned int stackSize)
  : m_hThread(NULL), m_pTarget(NULL)
{
  if (target != NULL) {
    Create(target, suspended, stackSize);
  }
}

CThread::~CThread(void) {
  if (m_hThread != NULL)
    Join(0);
}

unsigned int WINAPI CThread::ThreadProc(
    LPVOID lpParam)
{
  CThread* pThis = (CThread*)(lpParam);
  IRunnable* pRunnable = ((pThis != NULL) ? pThis->m_pTarget : NULL);

  ASSERT(pRunnable != NULL);

  unsigned int status = pRunnable->Run(*pThis);

  _endthreadex(status);

  return status;
}

bool CThread::Create(
    IRunnable* target,
    bool suspended,
    unsigned int stackSize)
{
  ASSERT(m_hThread == NULL);
  ASSERT(target != NULL);

  m_pTarget    = target;
  m_dwThreadID = 0;

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
    MSG* message)
{
  ASSERT(m_hThread != NULL);
  return (::GetMessage(message, NULL, 0, 0) != -1);
}
