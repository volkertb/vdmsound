// RUNWITHVDMSThread.cpp: implementation of the CRUNWITHVDMSThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "launchpad.h"
#include "RUNWITHVDMSThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRUNWITHVDMSThread::CRUNWITHVDMSThread(LPCTSTR fileName) : CWinThread(), m_settings(fileName)
{
  m_bAutoDelete = TRUE;     // let the thread manage its own life cycle
}

CRUNWITHVDMSThread::~CRUNWITHVDMSThread()
{
}

HRESULT CRUNWITHVDMSThread::CreateThread(LPCTSTR fileName) {
  // Create a self-destroying instance of CBasicSettingsPage
  CRUNWITHVDMSThread* pRUNWITHVDMSThread = new CRUNWITHVDMSThread(fileName);

  ASSERT_VALID(pRUNWITHVDMSThread);

  if (!pRUNWITHVDMSThread)
    return E_OUTOFMEMORY;

  _Module.Lock();               // prevent the DLL from being unloaded while the property sheet is active

  TRACE(_T("LaunchPad: about to start execution thread for '%s'\n"), fileName);
  TRACE(_T("LaunchPad: module lock count at start of execution thread is %d\n"), _Module.GetLockCount());

  if (!((CWinThread*)pRUNWITHVDMSThread)->CreateThread()) {
    _Module.Unlock();           // rollback
    delete pRUNWITHVDMSThread;
    return E_UNEXPECTED;
  }

  return S_OK;
}

BOOL CRUNWITHVDMSThread::InitInstance(void) {
  return TRUE;                  // enter Run()
}

int CRUNWITHVDMSThread::Run(void) {
  TRACE(_T("LaunchPad: execution thread started\n"));
  MessageBox(NULL, _T("zzz"), _T("zzz"), MB_OK);
  _Module.Unlock();             // release the DLL
  TRACE(_T("LaunchPad: module lock count after execution thread ended is %d\n"), _Module.GetLockCount());
  return 0;
}
