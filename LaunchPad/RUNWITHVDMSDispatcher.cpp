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
  CString fileName;

  // If not dealing with a .vlp file then try to find an associated .vlp file
  if (!VLPUtil::isVLPFile(m_fileName)) {
    fileName = VLPUtil::RenameExtension(m_fileName, _T(".vlp"));

    if (!VLPUtil::FileExists(fileName)) {
      return RunWithVdms_Wizard();
    }
  } else {
    fileName = m_fileName;
  }

  // Create and start a thread that spawns the 16-bit process and waits for its termination
  return CRUNWITHVDMSThread::CreateThread(fileName);
}

HRESULT CRUNWITHVDMSDispatcher::RunWithVdms_Wizard(void) {
  MessageBox(NULL, _T("of Oz"), _T("Wizard"), MB_OK);
  return E_NOTIMPL;
}
