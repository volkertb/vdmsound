// RUNWITHVDMSDispatcher.cpp: implementation of the CRUNWITHVDMSDispatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RUNWITHVDMSDispatcher.h"

#include "LaunchPadUtil.h"
#include "RUNWITHVDMSThread.h"

#include "resource.h"
#include "WizardSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRUNWITHVDMSDispatcher::CRUNWITHVDMSDispatcher(void)
{

}

CRUNWITHVDMSDispatcher::~CRUNWITHVDMSDispatcher(void)
{

}

//
// Attempts to run fileName if it is a .vlp file, or attempts to find a
//  surrogate .vlp if fileName is an executable, or fire up the VDMSound
//  Configuration Wizard if a surrogate .vlp file cannot be found.
// The execution is semi-synchronous (will return when either the child
//  process ends or when a timeour elapses, whichever takes less time).
//
HRESULT CRUNWITHVDMSDispatcher::RunWithVdms(LPCTSTR fileName) {
  CWaitCursor wait; // use wait (and not appwait) cursor because shell is actually blocked until we are done

  // If dealing with a .vlp file then start the program right away
  if (VLPUtil::isVLPFile(fileName)) {
    // Create and start a thread that spawns the 16-bit process and waits for its termination
    return CRUNWITHVDMSThread::CreateThread(
      VLPUtil::GetFilename(fileName, TRUE), // the .vlp file name, without extension, will be displayed in the title-bar
      CLaunchPadSettings(fileName),         // take the settings and executable name from the .vlp file
      NULL);                                // execute the executable specified in the .vlp's settings
  }

  ASSERT(VLPUtil::isMSDOSFile(fileName));

  CString vlpFileName;

  // If not dealing with a .vlp file then try to find an associated .vlp file
  if (VLPUtil::FileExists(vlpFileName = CString(fileName) + _T(".vlp")) ||
      VLPUtil::FileExists(vlpFileName = VLPUtil::RenameExtension(fileName, _T(".vlp"))))
  {
    return RunWithVdms(vlpFileName, fileName);
  }

  return RunWithVdms_Wizard(fileName);
}

//
// Uses vlpFile as a surrogate to run exeFileName.
// The execution is semi-synchronous (will return when either the child
//  process ends or when a timeour elapses, whichever takes less time).
//
HRESULT CRUNWITHVDMSDispatcher::RunWithVdms(LPCTSTR vlpFileName, LPCTSTR exeFileName) {
  CWaitCursor wait; // use wait (and not appwait) cursor because shell is actually blocked until we are done

  ASSERT(VLPUtil::isVLPFile(vlpFileName));
  ASSERT(VLPUtil::isMSDOSFile(exeFileName));

  // Create and start a thread that spawns the 16-bit process and waits for its termination
  return CRUNWITHVDMSThread::CreateThread(
    VLPUtil::GetFilename(exeFileName, FALSE), // the executable file name, with extension, will be displayed in the title-bar
    CLaunchPadSettings(vlpFileName),          // take the settings from the homonymous .vlp file
    exeFileName);                             // execute the executable using the homonymous .vlp's settings
}

HRESULT CRUNWITHVDMSDispatcher::RunWithVdms_Wizard(LPCTSTR exeFileName) {
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp;

  CString vlpFileName = VLPUtil::RenameExtension(exeFileName, _T(".vlp"));
  CLaunchPadSettings settings(vlpFileName);
/*
  settings.SetValue(_T("program"), _T("executable"), exeFileName);
  settings.SetValue(_T("program"), _T("params"), _T(""));
  settings.SetValue(_T("program"), _T("workdir"), VLPUtil::GetDirectory(exeFileName));
*/
  CWizardSheet wizardSheet(exeFileName, settings, IDS_PROJNAME);

  switch (wizardSheet.DoModal()) {
    case ID_WIZFINISH:
      break;

    case IDCANCEL:
      return S_FALSE;

    default:
      lastError = GetLastError();
      strTmp.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      MessageBox(NULL, strTmp, NULL, MB_OK | MB_ICONERROR);
      return E_FAIL;
  }

  // Create and start a thread that spawns the 16-bit process and waits for its termination
  return CRUNWITHVDMSThread::CreateThread(
    VLPUtil::GetFilename(exeFileName, TRUE),  // the executable file name, with extension, will be displayed in the title-bar
    settings,                                 // take the customized settings
    exeFileName);                             // execute the executable using the customized settings

  return S_OK;
}
