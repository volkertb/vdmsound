#include "stdafx.h"
#include "launchpad.h"
#include "RUNWITHVDMSThread.h"

#include "PIFFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
//
// CDOSEnv
//
//////////////////////////////////////////////////////////////////////

CDOSEnv::CDOSEnv() : CMapStringToString()
{
}

CDOSEnv::~CDOSEnv()
{
}

//
// Builds and returns an environment block based on the environment variables
//  present to date in CDOSEnv
//
BOOL CDOSEnv::GetEnvBlock(
  LPTSTR lpEnvBlock,
  int ncch) const
{
  ASSERT(!(ncch < 1));

  if (ncch < 1)
    return FALSE;

  // Zero out the environment block in case we are forced to return prematurely
  memset(lpEnvBlock, 0, ncch * sizeof(lpEnvBlock[0]));

  if (IsEmpty())
    return FALSE;

  // Add all entries we currently have in the environment map
  CString strKey, strValue;
  POSITION pos = GetStartPosition();

  while (pos != NULL) {
    GetNextAssoc(pos, strKey, strValue);

    CString strEnvEntry;
    strEnvEntry.Format(_T("%s=%s"), (LPCTSTR)strKey, (LPCTSTR)strValue);

    if (!AppendEnvBlockEntry(lpEnvBlock, strEnvEntry, ncch))
      return FALSE;
  }

  // Add a terminating empty environment entry
  if (!AppendEnvBlockEntry(lpEnvBlock, _T(""), ncch))
    return FALSE;

  return TRUE;                  // success
}

//
// Imports the current process' environment block
//
BOOL CDOSEnv::SetEnvBlock(void) {
  BOOL retVal = TRUE;

  LPTSTR lpEnvBlock = GetEnvironmentStrings();
  retVal &= SetEnvBlock(lpEnvBlock);
  retVal &= FreeEnvironmentStrings(lpEnvBlock);

  return retVal;
}

//
// Imports a given environment block that can then be manipulated
//
BOOL CDOSEnv::SetEnvBlock(
  LPCTSTR lpEnvBlock)
{
  // Empty the existing environment map
  RemoveAll();

  // Import the environment block entries into the environment map
  for (LPCTSTR lpEnvPos = lpEnvBlock; lpEnvPos[0] != _T('\0'); lpEnvPos += _tcslen(lpEnvPos) + 1) {
    CString strEnvEntry(lpEnvPos);
    int splitPos = strEnvEntry.Find(_T('='));

    if (splitPos < 0)
      return FALSE;

    SetAt(strEnvEntry.Left(splitPos), strEnvEntry.Mid(splitPos + 1));
  }

  return TRUE;
}

//
// Appends a 'var=value' entry to an existing environment block
//
BOOL CDOSEnv::AppendEnvBlockEntry(
  LPTSTR& lpEnvBlock,
  LPCTSTR szEntry,
  int& ncch)
{
  ASSERT(lpEnvBlock != NULL);
  ASSERT(szEntry);
  ASSERT(ncch > 0);

  if (ncch < 1)
    return FALSE;

  BOOL retVal;
  int cpyLen = _tcslen(szEntry) + 1;

  if (cpyLen > ncch) {
    cpyLen = ncch;
    retVal = FALSE;
  } else {
    retVal = TRUE;
  }

  memcpy(lpEnvBlock, szEntry, cpyLen * sizeof(lpEnvBlock[0]));
  lpEnvBlock += cpyLen;
  ncch       -= cpyLen;

  return retVal;
}



//////////////////////////////////////////////////////////////////////
//
// CRUNWITHVDMSThread
//
//////////////////////////////////////////////////////////////////////

#define THR_TIMEOUT 20000
#define MAX_ENVSIZE 65536

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

  CPIFFile PIFFile;
  TCHAR envBlock[MAX_ENVSIZE];

  if (SetupPIF(PIFFile) &&
      SetupEnv(envBlock, sizeof(envBlock) / sizeof(envBlock[0])))
  {
    CString strPIFFilename = PIFFile.GetFileName();

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_SHOWNORMAL;
    si.hStdInput   = NULL;
    si.hStdOutput  = NULL;
    si.hStdError   = NULL;

    DWORD dwCreationFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP | CREATE_SEPARATE_WOW_VDM;

#   ifdef _UNICODE
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
#   endif

    if (CreateProcess(NULL, strPIFFilename.GetBuffer(0), NULL, NULL, FALSE, dwCreationFlags, envBlock, NULL, &si, &pi)) {
      if (pi.hProcess != NULL) {
        WaitForSingleObject(pi.hProcess, THR_TIMEOUT);    // a valid handle was returned, wait for program to end
      } else {
        Sleep(THR_TIMEOUT);                               // no handle was available, wait max. amount of time
      }
    }
  }

  PIFFile.Delete();

  _Module.Unlock();             // release the DLL
  TRACE(_T("LaunchPad: module lock count after execution thread ended is %d\n"), _Module.GetLockCount());

  return 0;
}

BOOL CRUNWITHVDMSThread::SetupPIF(CPIFFile& PIFFile) {
  CString strTmp1, strTmp2;

  //
  // CONFIG
  //
  CString config;

  strTmp1.Format(_T("DOS=%s,%s\r\n"), SettingGetBool(_T("winnt.dos"), _T("useHIMEM.SYS")) ? _T("HIGH") : _T("LOW"),
                                    SettingGetBool(_T("winnt.dos"), _T("useUMB")) ? _T("UMB") : _T("NOUMB"));
  config += strTmp1;

  if (SettingGetBool(_T("winnt.dos"), _T("useUMB")) && SettingGetBool(_T("winnt.memory"), _T("useEMS"))) {
    config += _T("EMM=RAM\r\n");
  }

  if (SettingGetBool(_T("winnt.dos"), _T("useHIMEM.SYS"))) {
    config += _T("DEVICE=%SYSTEMROOT%\\SYSTEM32\\HIMEM.SYS\r\n");
  }

  strTmp1.Format(_T("FILES=%d\r\n"), SettingGetInt(_T("winnt.dos"), _T("files"), 40));
  config += strTmp1;

  //
  // AUTOEXEC
  //
  CString autoexec;

  autoexec += _T("@ECHO OFF\r\n");

  if (SettingGetBool(_T("winnt.storage"), _T("useCDROM"))) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\MSCDEXNT.EXE\r\n");
  }

  if (SettingGetBool(_T("winnt.storage"), _T("useNetware"))) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\REDIR\r\n");
  }

  if (SettingGetBool(_T("winnt.dos"), _T("useDPMI"))) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\DOSX\r\n");
  }

  if (SettingGetBool(_T("winnt.storage"), _T("useNetware"))) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\NW16\r\n");
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\VWIPXSPX\r\n");
  }

  //
  // PIF
  //
  PIFFile.SetWindowTitle(_T("VDMS Launchpad - DOS Prompt"));

  // Program
  PIFFile.SetProgram(SettingGetString(_T("program"), _T("executable")),
                     SettingGetString(_T("program"), _T("params")));
  PIFFile.SetWorkDir(SettingGetString(_T("program"), _T("workdir")));

  CString iconPath;
  int iconIndex;

  VLPUtil::ParseIconLocation(SettingGetString(_T("program"), _T("icon")), iconPath, iconIndex);

  PIFFile.SetIcon(iconPath, iconIndex);
  PIFFile.SetCloseOnExit(SettingGetBool(_T("winnt.dosbox"), _T("exitClose")));
  PIFFile.SetWarnOnClose(SettingGetBool(_T("winnt.dosbox"), _T("exitWarn")));

  PIFFile.SetFastPaste(SettingGetBool(_T("winnt.dosbox"), _T("fastPaste")));

  // Memory
  PIFFile.SetMemory(SettingGetBool(_T("winnt.memory"), _T("useEMS")) ? SettingGetInt(_T("winnt.memory"), _T("memEMS"), 16384) : 0,
                    SettingGetBool(_T("winnt.memory"), _T("useXMS")) ? SettingGetInt(_T("winnt.memory"), _T("memXMS"), 16384) : 0);

  // Screen
  PIFFile.SetFullScreen(SettingGetBool(_T("winnt.video"), _T("useVESA")));

  return PIFFile.Create(_T("VLP"), config, autoexec);
}

BOOL CRUNWITHVDMSThread::SetupEnv(LPTSTR lpEnvBlock, int ncch) {
  CDOSEnv DOSEnv;

  DOSEnv.SetEnvBlock();
  DOSEnv.SetAt(_T("BLASTER"), _T("BUUU"));

  return DOSEnv.GetEnvBlock(lpEnvBlock, ncch);
}

CString CRUNWITHVDMSThread::SettingGetString(LPCTSTR section, LPCTSTR key, LPCTSTR defValue) {
  CString retVal(defValue);
  m_settings.GetValue(section, key, retVal, NULL, defValue);
  return retVal;
}

bool CRUNWITHVDMSThread::SettingGetBool(LPCTSTR section, LPCTSTR key, bool defValue) {
  CString strDefValue(defValue ? VLPUtil::T_YES : VLPUtil::T_NO);
  CString strRetVal(strDefValue);

  m_settings.GetValue(section, key, strRetVal, NULL, strDefValue);
  strRetVal.TrimLeft(); strRetVal.TrimRight();

  if (strRetVal.CompareNoCase(VLPUtil::T_YES) == 0) {
    return true;
  } else if (strRetVal.CompareNoCase(VLPUtil::T_NO) == 0) {
    return false;
  } else {
    return defValue;
  }
}

int CRUNWITHVDMSThread::SettingGetInt(LPCTSTR section, LPCTSTR key, int defValue) {
  long retVal;
  CString strDefValue; strDefValue.Format(_T("%d"), defValue);
  CString strRetVal(strDefValue);

  m_settings.GetValue(section, key, strRetVal, NULL, strDefValue);
  strRetVal.TrimLeft(); strRetVal.TrimRight();

  TCHAR* endPtr = NULL;
  retVal = _tcstol(strRetVal, &endPtr, 0);

  if ((endPtr != NULL) && (endPtr[0] == _T('\0'))) {
    return retVal;
  } else {
    return defValue;
  }
}
