// TipOfDay.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TipOfDay.h"
#include "TipOfDayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CTipOfDayApp

BEGIN_MESSAGE_MAP(CTipOfDayApp, CWinApp)
	//{{AFX_MSG_MAP(CTipOfDayApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipOfDayApp construction

CTipOfDayApp::CTipOfDayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTipOfDayApp object

CTipOfDayApp theApp;

/////////////////////////////////////////////////////////////////////////////
//
// Member functions
//
/////////////////////////////////////////////////////////////////////////////

DWORD CTipOfDayApp::Initialize(const CString& keyName) {
  TCHAR szBuf[_MAX_PATH + 1];
  DWORD dwBufLen = _MAX_PATH;
  DWORD dwErrCode;

  if (((dwErrCode = m_cfgKey.CreateDeep(HKEY_CURRENT_USER, keyName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_WRITE)) != ERROR_SUCCESS) ||
      ((dwErrCode = m_cfgKey.QueryValue(szBuf, _T("TipsFile"), &dwBufLen)) != ERROR_SUCCESS))
  {
    return dwErrCode;
  }

  if ((dwErrCode = m_tipFile.LoadTips(szBuf)) != ERROR_SUCCESS)
    return dwErrCode;

  if (m_tipFile.GetNumTips() < 1)
    return ERROR_HANDLE_EOF;

  return ERROR_SUCCESS;
}

DWORD CTipOfDayApp::IsEnabled(DWORD& isEnabled) {
  return CfgGet_ShowTips(isEnabled);
}

DWORD CTipOfDayApp::ShowTips(void) {
  CTipOfDayDlg dialog;
  DWORD showTips = 1, nextTip = 0;

  CfgGet_ShowTips(showTips);
  CfgGet_NextTip(nextTip);

  dialog.SetShowStartup(showTips);
  dialog.SetTipID(nextTip);

  dialog.SetTipFile(m_tipFile);

  switch (dialog.DoModal()) {
    case IDOK:
      CfgSet_ShowTips(dialog.GetShowStartup());
      CfgSet_NextTip((dialog.GetTipID() + 1) % m_tipFile.GetNumTips());
      break;

    default:
      break;
  }

  return ERROR_SUCCESS;
}

DWORD CTipOfDayApp::CfgGet_ShowTips(DWORD& dwValue) {
  return m_cfgKey.QueryValue(dwValue, _T("ShowTips"));
}

DWORD CTipOfDayApp::CfgGet_NextTip(DWORD& dwValue) {
  return m_cfgKey.QueryValue(dwValue, _T("NextTip"));
}

DWORD CTipOfDayApp::CfgSet_ShowTips(DWORD dwValue) {
  return m_cfgKey.SetValue(dwValue, _T("ShowTips"));
}

DWORD CTipOfDayApp::CfgSet_NextTip(DWORD dwValue) {
  return m_cfgKey.SetValue(dwValue, _T("NextTip"));
}

/////////////////////////////////////////////////////////////////////////////
//
// Exported functions (entrypoints)
//
/////////////////////////////////////////////////////////////////////////////

//
// Initialize the control
//
BOOL APIENTRY TODInitializeA(LPCSTR keyNameA) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  DWORD dwErrCode = theApp.Initialize(CString(keyNameA));

  if (dwErrCode != ERROR_SUCCESS) {
    SetLastError(dwErrCode);
    return FALSE;
  } else {
    return TRUE;
  }
}

BOOL APIENTRY TODInitializeW(LPCWSTR keyNameW) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  DWORD dwErrCode = theApp.Initialize(CString(keyNameW));

  if (dwErrCode != ERROR_SUCCESS) {
    SetLastError(dwErrCode);
    return FALSE;
  } else {
    return TRUE;
  }
}

//
// Report whether the tips should be shown or not (based on user's wish)
//
BOOL APIENTRY TODIsEnabled(BOOL* pIsEnabled) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  if (pIsEnabled == NULL)
    return FALSE;
 
  DWORD isEnabled = FALSE;
  DWORD dwErrCode = theApp.IsEnabled(isEnabled);

  if (dwErrCode != ERROR_SUCCESS) {
    SetLastError(dwErrCode);
    return FALSE;
  } else {
    *pIsEnabled = (isEnabled != 0);
    return TRUE;
  }
}

//
// Show the "Tips of the day" dialog
//
BOOL APIENTRY TODShowTips(void) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  DWORD dwErrCode = theApp.ShowTips();

  if (dwErrCode != ERROR_SUCCESS) {
    SetLastError(dwErrCode);
    return FALSE;
  } else {
    return TRUE;
  }
}

void APIENTRY TODShowTipsRunDLL(
  HWND hwndStub,
  HINSTANCE hAppInstance,
  LPSTR lpszCmdLine,
  int nCmdShow )
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  theApp.Initialize(lpszCmdLine);
  theApp.ShowTips();
}
