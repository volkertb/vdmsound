// RUNWITHVDMSDispatcher.cpp: implementation of the CRUNWITHVDMSDispatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RUNWITHVDMSDispatcher.h"

#include "LaunchPadUtil.h"
#include "RUNWITHVDMSThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRUNWITHVDMSDispatcher::CRUNWITHVDMSDispatcher(CString fileName) : m_fileName(fileName)
{

}

CRUNWITHVDMSDispatcher::~CRUNWITHVDMSDispatcher()
{

}

HRESULT CRUNWITHVDMSDispatcher::RunWithVdms(void) {
  CWaitCursor wait; // use wait (and not appwait) cursor because shell is actually blocked until we are done

  // If dealing with a .vlp file then start the program right away
  if (VLPUtil::isVLPFile(m_fileName)) {
    return CRUNWITHVDMSThread::CreateThread(m_fileName);  // create and start a thread that spawns the 16-bit process and waits for its termination
  }

  CString vlpFileName;

  // If not dealing with a .vlp file then try to find an associated .vlp file
  if (VLPUtil::FileExists(vlpFileName = m_fileName + _T(".vlp")) ||
      VLPUtil::FileExists(vlpFileName = VLPUtil::RenameExtension(m_fileName, _T(".vlp"))))
  {
    return CRUNWITHVDMSThread::CreateThread(vlpFileName, m_fileName); // create and start a thread that spawns the 16-bit process and waits for its termination
  }

  return RunWithVdms_Wizard();
}

HRESULT CRUNWITHVDMSDispatcher::RunWithVdms_Wizard(void) {
  MessageBox(NULL, _T("of Oz"), _T("Wizard"), MB_OK);

  // TODO: implement VDMS LaunchPad wizard

  return E_NOTIMPL;
}
