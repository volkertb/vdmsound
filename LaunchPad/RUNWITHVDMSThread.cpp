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
  CINIFile INIFile;
  TCHAR envBlock[MAX_ENVSIZE];

  if (SetupINI(INIFile) &&
      SetupPIF(PIFFile, INIFile) &&
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

//
//
//
BOOL CRUNWITHVDMSThread::SetupINI(CINIFile& INIFile) {
  CString vdmsini;

  CString vdms_debug_detail         = SettingGetString(_T("vdms.debug"), _T("detail"), _T("256"));
  CString vdms_debug_file           = SettingGetString(_T("vdms.debug"), _T("file"), _T(".\\VDMS.LOG"));

  BOOL    vdms_midi_enabled         = SettingGetBool  (_T("vdms.midi"), _T("enabled"), TRUE);
  CString vdms_midi_port            = SettingGetString(_T("vdms.midi"), _T("port"), _T("0x330"));
  CString vdms_midi_IRQ             = SettingGetString(_T("vdms.midi"), _T("IRQ"), _T("2"));
  CString vdms_midi_mapFile         = SettingGetString(_T("vdms.midi"), _T("mapFile"), _T("identity.map"));
  CString vdms_midi_device          = SettingGetString(_T("vdms.midi"), _T("device"), _T("-1"));
  CString vdms_midi_showSysExLed    = SettingGetString(_T("vdms.midi"), _T("showSysExLed"), _T("Scroll"));

  BOOL    vdms_sb_dsp_enabled       = SettingGetBool  (_T("vdms.sb.dsp"), _T("enabled"), TRUE);
  CString vdms_sb_dsp_port          = SettingGetString(_T("vdms.sb.dsp"), _T("port"), _T("0x220"));
  CString vdms_sb_dsp_IRQ           = SettingGetString(_T("vdms.sb.dsp"), _T("IRQ"), _T("7"));
  CString vdms_sb_dsp_DMA8          = SettingGetString(_T("vdms.sb.dsp"), _T("DMA8"), _T("1"));
  CString vdms_sb_dsp_DMA16         = SettingGetString(_T("vdms.sb.dsp"), _T("DMA16"), _T("5"));
  CString vdms_sb_dsp_version       = SettingGetString(_T("vdms.sb.dsp"), _T("version"), _T("4.15"));
  CString vdms_sb_dsp_minDMAPeriod  = SettingGetString(_T("vdms.sb.dsp"), _T("minDMAPeriod"), _T("5"));
  CString vdms_sb_dsp_maxDMAPeriod  = SettingGetString(_T("vdms.sb.dsp"), _T("maxDMAPeriod"), _T("15"));
  CString vdms_sb_dsp_device        = SettingGetString(_T("vdms.sb.dsp"), _T("device"), _T("-1"));
  CString vdms_sb_dsp_buffer        = SettingGetString(_T("vdms.sb.dsp"), _T("buffer"), _T("75"));

  BOOL    vdms_sb_fm_enabled        = SettingGetBool  (_T("vdms.sb.fm"), _T("enabled"), TRUE);
  CString vdms_sb_fm_port           = SettingGetString(_T("vdms.sb.fm"), _T("port"), _T("0x388"));
  CString vdms_sb_fm_sampleRate     = SettingGetString(_T("vdms.sb.fm"), _T("sampleRate"), _T("11025"));
  CString vdms_sb_fm_device         = SettingGetString(_T("vdms.sb.fm"), _T("device"), _T("-1"));
  CString vdms_sb_fm_buffer         = SettingGetString(_T("vdms.sb.fm"), _T("buffer"), _T("75"));

  BOOL    vdms_gameport_enabled     = SettingGetBool  (_T("vdms.gameport"), _T("enabled"), TRUE);
  CString vdms_gameport_port        = SettingGetString(_T("vdms.gameport"), _T("port"), _T("0x201"));
  CString vdms_gameport_minCoord    = SettingGetString(_T("vdms.gameport"), _T("minCoord"), _T("5"));
  CString vdms_gameport_maxCoord    = SettingGetString(_T("vdms.gameport"), _T("maxCoord"), _T("250"));
  CString vdms_gameport_pollPeriod  = SettingGetString(_T("vdms.gameport"), _T("pollPeriod"), _T("125"));
  CString vdms_gameport_mapFile     = SettingGetString(_T("vdms.gameport"), _T("mapFile"), _T("joy2.map"));

  //
  // Basic settings
  //
  vdmsini += _T("[*.debug]\n");
  vdmsini += _T("detail=") + vdms_debug_detail + _T("\n");
  vdmsini += _T("file=") + vdms_debug_file + _T("\n");

  vdmsini += _T("[VDMServicesProvider]\n");
  vdmsini += _T("CLSID=VDDLoader.VDMServices\n");

  //
  // MIDI
  //
  if (vdms_midi_enabled) {
    vdmsini += _T("[MPU401Controller]\n");
    vdmsini += _T("CLSID=EmuMPU401.MPU401Ctl\n");
    vdmsini += _T("[MPU401Controller.config]\n");
    vdmsini += _T("port=") + vdms_midi_port + _T("\n");
    vdmsini += _T("IRQ=") + vdms_midi_IRQ + _T("\n");
    vdmsini += _T("[MPU401Controller.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");
    vdmsini += _T("MidiOut=MIDIMapper\n");

    vdmsini += _T("[MIDIMapper]\n");
    vdmsini += _T("CLSID=MIDIToolkit.MIDIMapper\n");
    vdmsini += _T("[MIDIMapper.config]\n");
    vdmsini += _T("mapFile=") + vdms_midi_mapFile + _T("\n");
    vdmsini += _T("[MIDIMapper.depends]\n");
    vdmsini += _T("MidiOut=MIDIPlayer\n");

    vdmsini += _T("[MIDIPlayer]\n");
    vdmsini += _T("CLSID=MIDIDevice.MIDIOut\n");
    vdmsini += _T("[MIDIPlayer.config]\n");
    vdmsini += _T("device=") + vdms_midi_device + _T("\n");
    vdmsini += _T("[MIDIPlayer.depends]\n");
    vdmsini += _T("MidiOut=SysExIndicator\n");

    vdmsini += _T("[SysExIndicator]\n");
    vdmsini += _T("CLSID=MIDIIndicator.ActivityLights\n");
    vdmsini += _T("[SysExIndicator.config]\n");
    vdmsini += _T("led=") + vdms_midi_showSysExLed + _T("\n");
  }

  //
  // SB - digital
  //
  if (vdms_sb_dsp_enabled) {
    vdmsini += _T("[DMATransferManager]\n");
    vdmsini += _T("CLSID=DMAController.TransferMgr\n");
    vdmsini += _T("[DMATransferManager.config]\n");
    vdmsini += _T("minDMAPeriod=") + vdms_sb_dsp_minDMAPeriod + _T("\n");
    vdmsini += _T("maxDMAPeriod=") + vdms_sb_dsp_maxDMAPeriod + _T("\n");
    vdmsini += _T("[DMATransferManager.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");

    vdmsini += _T("[SBController]\n");
    vdmsini += _T("CLSID=EmuSBCompat.SBCompatCtl\n");
    vdmsini += _T("[SBController.config]\n");
    vdmsini += _T("version=") + vdms_sb_dsp_version + _T("\n");
    vdmsini += _T("port=") + vdms_sb_dsp_port + _T("\n");
    vdmsini += _T("IRQ=") + vdms_sb_dsp_IRQ + _T("\n");
    vdmsini += _T("DMA8=") + vdms_sb_dsp_DMA8 + _T("\n");
    vdmsini += _T("DMA16=") + vdms_sb_dsp_DMA16 + _T("\n");
    vdmsini += _T("[SBController.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");
    vdmsini += _T("DMACtl=DMATransferManager\n");
    vdmsini += _T("WaveOut=SBWavePlayer\n");

    if (vdms_sb_fm_enabled) {
      vdmsini += _T("AdLib=AdLibController\n");
    }

    vdmsini += _T("[SBWavePlayer]\n");
    vdmsini += _T("CLSID=DSoundDevice.WaveOut\n");
    vdmsini += _T("[SBWavePlayer.config]\n");
    vdmsini += _T("device=") + vdms_sb_dsp_device + _T("\n");
    vdmsini += _T("buffer=") + vdms_sb_dsp_buffer + _T("\n");
  }

  //
  // SB - FM (AdLib)
  //
  if (vdms_sb_fm_enabled) {
    vdmsini += _T("[AdLibController]\n");
    vdmsini += _T("CLSID=EmuAdLib.AdLibCtl\n");
    vdmsini += _T("[AdLibController.config]\n");
    vdmsini += _T("port=") + vdms_sb_fm_port + _T("\n");
    vdmsini += _T("sampleRate=") + vdms_sb_fm_sampleRate + _T("\n");
    vdmsini += _T("[AdLibController.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");
    vdmsini += _T("WaveOut=AdLibWavePlayer\n");

    vdmsini += _T("[AdLibWavePlayer]\n");
    vdmsini += _T("CLSID=DSoundDevice.WaveOut\n");
    vdmsini += _T("[AdLibWavePlayer.config]\n");
    vdmsini += _T("device=") + vdms_sb_fm_device + _T("\n");
    vdmsini += _T("buffer=") + vdms_sb_fm_buffer + _T("\n");
  }

  //
  // Joystick
  //
  if (vdms_gameport_enabled) {
    vdmsini += _T("[JoystickController]\n");
    vdmsini += _T("CLSID=EmuJoystick.JoystickCtl\n");
    vdmsini += _T("[JoystickController.config]\n");
    vdmsini += _T("port=") + vdms_gameport_port + _T("\n");
    vdmsini += _T("minCoord=") + vdms_gameport_minCoord + _T("\n");
    vdmsini += _T("maxCoord=") + vdms_gameport_maxCoord + _T("\n");
    vdmsini += _T("pollPeriod=") + vdms_gameport_pollPeriod + _T("\n");
    vdmsini += _T("mapFile=") + vdms_gameport_mapFile + _T("\n");
    vdmsini += _T("[JoystickController.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");
  }

  return INIFile.Create(vdmsini);
}

//
//
//
BOOL CRUNWITHVDMSThread::SetupPIF(CPIFFile& PIFFile, CINIFile& INIFile) {
  CString strTmp;

  //
  // CONFIG
  //
  CString config;

  BOOL vdms_winnt_dos_useHIMEM  = SettingGetBool(_T("winnt.dos"), _T("useHIMEM.SYS"), TRUE);
  BOOL vdms_winnt_dos_useUMB    = SettingGetBool(_T("winnt.dos"), _T("useUMB"), TRUE);
  BOOL vdms_winnt_memory_useEMS = SettingGetBool(_T("winnt.memory"), _T("useEMS"), TRUE);

  strTmp.Format(_T("DOS=%s,%s\r\n"), vdms_winnt_dos_useHIMEM ? _T("HIGH") : _T("LOW"),
                                     vdms_winnt_dos_useUMB   ? _T("UMB")  : _T("NOUMB"));
  config += strTmp;

  if (vdms_winnt_dos_useUMB && vdms_winnt_memory_useEMS) {
    config += _T("EMM=RAM\r\n");
  }

  if (vdms_winnt_dos_useHIMEM) {
    config += _T("DEVICE=%SYSTEMROOT%\\SYSTEM32\\HIMEM.SYS\r\n");
  }

  // TODO: add "custom" CONFIG.SYS options from VDMS.INI to config

  //
  // AUTOEXEC
  //
  CString autoexec;

  BOOL vdms_winnt_storage_useCDROM   = SettingGetBool(_T("winnt.storage"), _T("useCDROM"), FALSE);
  BOOL vdms_winnt_storage_useNetware = SettingGetBool(_T("winnt.storage"), _T("useNetware"), FALSE);
  BOOL vdms_winnt_pmode_useDPMI      = SettingGetBool(_T("winnt.pmode"), _T("useDPMI"), TRUE);

  autoexec += _T("@ECHO OFF\r\n");

  if (vdms_winnt_storage_useCDROM) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\MSCDEXNT.EXE\r\n");
  }

  if (vdms_winnt_storage_useNetware) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\REDIR\r\n");
  }

  if (vdms_winnt_pmode_useDPMI) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\DOSX\r\n");
  }

  if (vdms_winnt_storage_useNetware) {
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\NW16\r\n");
    autoexec += _T("LH %SYSTEMROOT%\\SYSTEM32\\VWIPXSPX\r\n");
  }

  // TODO: add "custom" AUTOEXEC.BAT options from VDMS.INI to autoexec

  autoexec += VLPUtil::GetAbsolutePath(_T("dosdrv.exe"), _tgetenv(_T("VDMSPath"))) + _T(" \"-i:") + INIFile.GetFileName() + _T("\"\r\n");

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

  return PIFFile.Create(config, autoexec);
}

//
//
//
BOOL CRUNWITHVDMSThread::SetupEnv(LPTSTR lpEnvBlock, int ncch) {
  CDOSEnv DOSEnv;

  DOSEnv.SetEnvBlock();
  DOSEnv.SetAt(_T("BLASTER"), _T("BUUU"));

  return DOSEnv.GetEnvBlock(lpEnvBlock, ncch);
}

//
//
//
CString CRUNWITHVDMSThread::SettingGetString(LPCTSTR section, LPCTSTR key, LPCTSTR defValue) {
  CString retVal(defValue);
  m_settings.GetValue(section, key, retVal, NULL, defValue);
  return retVal;
}

//
//
//
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

//
//
//
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
