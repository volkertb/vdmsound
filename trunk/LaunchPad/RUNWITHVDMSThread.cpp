#include "stdafx.h"
#include "resource.h"
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

CRUNWITHVDMSThread::CRUNWITHVDMSThread(LPCTSTR displayName, const CLaunchPadSettings& settings, LPCTSTR exeFileName) : CWinThread(), m_displayName(displayName), m_exeFileName(exeFileName), m_settings(settings)
{
  m_bAutoDelete = TRUE;     // let the thread manage its own life cycle

  if (exeFileName != NULL) {
    // Override .vlp settings for program name, args and dir during this session,
    //  but do not commit to disk
    m_settings.SetValue(_T("program"), _T("executable"), exeFileName);
    m_settings.SetValue(_T("program"), _T("params"), _T(""));
    m_settings.SetValue(_T("program"), _T("workdir"), VLPUtil::GetDirectory(exeFileName));
  }
}

CRUNWITHVDMSThread::~CRUNWITHVDMSThread()
{
}

HRESULT CRUNWITHVDMSThread::CreateThread(LPCTSTR displayName, const CLaunchPadSettings& settings, LPCTSTR exeFileName) {
  // Create a self-destroying instance of CBasicSettingsPage
  CRUNWITHVDMSThread* pRUNWITHVDMSThread = new CRUNWITHVDMSThread(displayName, settings, exeFileName);

  ASSERT_VALID(pRUNWITHVDMSThread);

  if (!pRUNWITHVDMSThread)
    return E_OUTOFMEMORY;

  _Module.Lock();               // prevent the DLL from being unloaded while the "Run with VDMS" thread is active

  TRACE(_T("LaunchPad: about to start execution thread for '%s' ('%s')\n"), displayName, exeFileName ? exeFileName : _T("<not specified>"));
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

    // TODO: disable VDMSound tip-of-the-day from registry
    //  (to be restored when the temporary files are deleted)

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_SHOWNORMAL;
    si.hStdInput   = NULL;  // NULL seems to be the only value that works when spawning
    si.hStdOutput  = NULL;  //  command.com or cmd.exe even when/if I inherit handles
    si.hStdError   = NULL;  //  in CreateProcess -- WHY?!

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

  const CString str_identity_map = VLPUtil::GetVDMSFilePath(_T("identity.map"));

  const int log_none     = 256;
  const int log_errors   = 192;
  const int log_warnings = 128;
  const int log_all      =   0;

  BOOL    vdms_debug_logenabled     = SettingGetBool  (_T("vdms.debug"),    _T("logEnabled"),   FALSE);
  LONG    vdms_debug_logdetail      = SettingGetLong  (_T("vdms.debug"),    _T("logDetail"),    log_all);
  CString vdms_debug_logfile        = SettingGetString(_T("vdms.debug"),    _T("logFile"),      _T(".\\VDMS.LOG"));
  BOOL    vdms_debug_logmidi        = SettingGetBool  (_T("vdms.debug"),    _T("logMidi"),      TRUE);
  BOOL    vdms_debug_logadlib       = SettingGetBool  (_T("vdms.debug"),    _T("logAdLib"),     TRUE);
  BOOL    vdms_debug_logsblaster    = SettingGetBool  (_T("vdms.debug"),    _T("logSBlaster"),  TRUE);
  BOOL    vdms_debug_logjoystick    = SettingGetBool  (_T("vdms.debug"),    _T("logJoystick"),  TRUE);
  BOOL    vdms_debug_logother       = SettingGetBool  (_T("vdms.debug"),    _T("logOther"),     TRUE);
  BOOL    vdms_debug_usecustomcfg   = SettingGetBool  (_T("vdms.debug"),    _T("useCustomCfg"), TRUE);
  CString vdms_debug_customcfg      = SettingGetString(_T("vdms.debug"),    _T("customCfg"),    _T(""));

  BOOL    vdms_midi_enabled         = SettingGetBool  (_T("vdms.midi"),     _T("enabled"),      TRUE);
  LONG    vdms_midi_port            = SettingGetLong  (_T("vdms.midi"),     _T("port"),         0x330);
  LONG    vdms_midi_IRQ             = SettingGetLong  (_T("vdms.midi"),     _T("IRQ"),          2);
  CString vdms_midi_mapFile         = SettingGetString(_T("vdms.midi"),     _T("mapFile"),      str_identity_map);
  BOOL    vdms_midi_useDevOut       = SettingGetBool  (_T("vdms.midi"),     _T("useDevOut"),    TRUE);
  LONG    vdms_midi_devOutType      = SettingGetLong  (_T("vdms.midi"),     _T("devOutType"),   (LONG)DeviceUtil::DEV_MIDI);
  LONG    vdms_midi_devOutID        = SettingGetLong  (_T("vdms.midi"),     _T("devOutID"),     -1);
  BOOL    vdms_midi_useSysExLed     = SettingGetBool  (_T("vdms.midi"),     _T("useSysExLed"),  TRUE);
  CString vdms_midi_sysExLed        = SettingGetString(_T("vdms.midi"),     _T("sysExLed"),     _T("scroll"));

  LONG    vdms_dmac_minDMAPeriod    = SettingGetLong  (_T("vdms.dmac"),     _T("minDMAPeriod"), 5);
  LONG    vdms_dmac_maxDMAPeriod    = SettingGetLong  (_T("vdms.dmac"),     _T("maxDMAPeriod"), 15);

  BOOL    vdms_sb_dsp_enabled       = SettingGetBool  (_T("vdms.sb.dsp"),   _T("enabled"),      TRUE);
  LONG    vdms_sb_dsp_port          = SettingGetLong  (_T("vdms.sb.dsp"),   _T("port"),         0x220);
  LONG    vdms_sb_dsp_IRQ           = SettingGetLong  (_T("vdms.sb.dsp"),   _T("IRQ"),          7);
  LONG    vdms_sb_dsp_DMA8          = SettingGetLong  (_T("vdms.sb.dsp"),   _T("DMA8"),         1);
  LONG    vdms_sb_dsp_DMA16         = SettingGetLong  (_T("vdms.sb.dsp"),   _T("DMA16"),        5);
  CString vdms_sb_dsp_version       = SettingGetString(_T("vdms.sb.dsp"),   _T("version"),      _T("4.05 (SoundBlaster 16)"));
  BOOL    vdms_sb_dsp_useDevOut     = SettingGetLong  (_T("vdms.sb.dsp"),   _T("useDevOut"),    TRUE);
  LONG    vdms_sb_dsp_devOutType    = SettingGetLong  (_T("vdms.sb.dsp"),   _T("devOutType"),   (LONG)DeviceUtil::DEV_DSOUND);
  LONG    vdms_sb_dsp_devOutID      = SettingGetLong  (_T("vdms.sb.dsp"),   _T("devOutID"),     -1);
  LONG    vdms_sb_dsp_buffer        = SettingGetLong  (_T("vdms.sb.dsp"),   _T("buffer"),       75);

  BOOL    vdms_sb_fm_enabled        = SettingGetBool  (_T("vdms.sb.fm"),    _T("enabled"),      TRUE);
  LONG    vdms_sb_fm_port           = SettingGetLong  (_T("vdms.sb.fm"),    _T("port"),         0x388);
  LONG    vdms_sb_fm_sampleRate     = SettingGetLong  (_T("vdms.sb.fm"),    _T("sampleRate"),   11025);
  BOOL    vdms_sb_fm_useDevOut      = SettingGetLong  (_T("vdms.sb.fm"),    _T("useDevOut"),    TRUE);
  LONG    vdms_sb_fm_devOutType     = SettingGetLong  (_T("vdms.sb.fm"),    _T("devOutType"),   (LONG)DeviceUtil::DEV_DSOUND);
  LONG    vdms_sb_fm_devOutID       = SettingGetLong  (_T("vdms.sb.fm"),    _T("devOutID"),     -1);
  LONG    vdms_sb_fm_buffer         = SettingGetLong  (_T("vdms.sb.fm"),    _T("buffer"),       75);

  BOOL    vdms_gameport_enabled     = SettingGetBool  (_T("vdms.gameport"), _T("enabled"),      TRUE);
  LONG    vdms_gameport_port        = SettingGetLong  (_T("vdms.gameport"), _T("port"),         0x201);
  LONG    vdms_gameport_minCoord    = SettingGetLong  (_T("vdms.gameport"), _T("minCoord"),     5);
  LONG    vdms_gameport_maxCoord    = SettingGetLong  (_T("vdms.gameport"), _T("maxCoord"),     250);
  LONG    vdms_gameport_pollPeriod  = SettingGetLong  (_T("vdms.gameport"), _T("pollPeriod"),   125);
  CString vdms_gameport_mapFile     = SettingGetString(_T("vdms.gameport"), _T("mapFile"),      VLPUtil::GetVDMSFilePath(_T("joy2.map")));

  //
  // Custom configuration
  //
  if (vdms_debug_usecustomcfg) {
    vdmsini += vdms_debug_customcfg + _T("\n");   // begin with the custom configuration so that it is easy to map line numbers in case of an error, i.e. "line 1 in VDMS.INI" is line 1 in the customCfg blob
  }

  //
  // Basic settings
  //
  vdmsini += _T("[*.debug]\n");
  vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logother) ? vdms_debug_logdetail : log_none) + _T("\n");
  vdmsini += _T("file=") + vdms_debug_logfile + _T("\n");

  vdmsini += _T("[VDMServicesProvider]\n");
  vdmsini += _T("CLSID=VDDLoader.VDMServices\n");

  //
  // MIDI
  //
  if (vdms_midi_enabled) {
    vdmsini += _T("[MPU401Controller]\n");
    vdmsini += _T("CLSID=EmuMPU401.MPU401Ctl\n");
    vdmsini += _T("[MPU401Controller.debug]\n");
    vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logmidi) ? vdms_debug_logdetail : log_none) + _T("\n");
    vdmsini += _T("[MPU401Controller.config]\n");
    vdmsini += _T("port=") + VLPUtil::FormatString(_T("0x%x"), vdms_midi_port) + _T("\n");
    vdmsini += _T("IRQ=") + VLPUtil::FormatString(_T("%d"), vdms_midi_IRQ) + _T("\n");
    vdmsini += _T("[MPU401Controller.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");

    if (vdms_midi_mapFile.CompareNoCase(str_identity_map) != 0) {
      vdmsini += _T("MidiOut=MIDIMapper\n");

      vdmsini += _T("[MIDIMapper]\n");
      vdmsini += _T("CLSID=MIDIToolkit.MIDIMapper\n");
      vdmsini += _T("[MIDIMapper.debug]\n");
      vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logmidi) ? vdms_debug_logdetail : log_none) + _T("\n");
      vdmsini += _T("[MIDIMapper.config]\n");
      vdmsini += _T("mapFile=") + vdms_midi_mapFile + _T("\n");
      vdmsini += _T("[MIDIMapper.depends]\n");
    }

    if (vdms_midi_useDevOut) {
      vdmsini += _T("MidiOut=MIDIPlayer\n");

      vdmsini += _T("[MIDIPlayer]\n");
      vdmsini += _T("CLSID=") + GetDeviceID((DeviceUtil::DeviceType)vdms_midi_devOutType) + _T(".MIDIOut\n");
      vdmsini += _T("[MIDIPlayer.debug]\n");
      vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logmidi) ? vdms_debug_logdetail : log_none) + _T("\n");
      vdmsini += _T("[MIDIPlayer.config]\n");
      vdmsini += _T("device=") + VLPUtil::FormatString(_T("%d"), vdms_midi_devOutID) + _T("\n");
      vdmsini += _T("[MIDIPlayer.depends]\n");
    }

    if (vdms_midi_useSysExLed) {
      vdmsini += _T("MidiOut=SysExIndicator\n");

      vdmsini += _T("[SysExIndicator]\n");
      vdmsini += _T("CLSID=MIDIIndicator.ActivityLights\n");
      vdmsini += _T("[SysExIndicator.debug]\n");
      vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logmidi) ? vdms_debug_logdetail : log_none) + _T("\n");
      vdmsini += _T("[SysExIndicator.config]\n");
      vdmsini += _T("led=") + vdms_midi_sysExLed + _T("\n");
    }
  }

  //
  // SB - digital
  //
  if (vdms_sb_dsp_enabled) {
    vdmsini += _T("[DMATransferManager]\n");
    vdmsini += _T("CLSID=DMAController.TransferMgr\n");
    vdmsini += _T("[DMATransferManager.debug]\n");
    vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logsblaster) ? vdms_debug_logdetail : log_none) + _T("\n");
    vdmsini += _T("[DMATransferManager.config]\n");
    vdmsini += _T("minDMAPeriod=") + VLPUtil::FormatString(_T("%d"), vdms_dmac_minDMAPeriod) + _T("\n");
    vdmsini += _T("maxDMAPeriod=") + VLPUtil::FormatString(_T("%d"), vdms_dmac_maxDMAPeriod) + _T("\n");
    vdmsini += _T("[DMATransferManager.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");

    vdmsini += _T("[SBController]\n");
    vdmsini += _T("CLSID=EmuSBCompat.SBCompatCtl\n");
    vdmsini += _T("[SBController.debug]\n");
    vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logsblaster) ? vdms_debug_logdetail : log_none) + _T("\n");
    vdmsini += _T("[SBController.config]\n");
    vdmsini += _T("version=") + vdms_sb_dsp_version + _T("\n");
    vdmsini += _T("port=") + VLPUtil::FormatString(_T("0x%x"), vdms_sb_dsp_port) + _T("\n");
    vdmsini += _T("IRQ=") + VLPUtil::FormatString(_T("%d"), vdms_sb_dsp_IRQ) + _T("\n");
    vdmsini += _T("DMA8=") + VLPUtil::FormatString(_T("%d"), vdms_sb_dsp_DMA8) + _T("\n");
    vdmsini += _T("DMA16=") + VLPUtil::FormatString(_T("%d"), vdms_sb_dsp_DMA16) + _T("\n");
    vdmsini += _T("[SBController.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");
    vdmsini += _T("DMACtl=DMATransferManager\n");

    if (vdms_sb_fm_enabled) {
      vdmsini += _T("AdLib=AdLibController\n");
    }

    if (vdms_sb_dsp_useDevOut) {
      vdmsini += _T("WaveOut=SBWavePlayer\n");

      vdmsini += _T("[SBWavePlayer]\n");
      vdmsini += _T("CLSID=") + GetDeviceID((DeviceUtil::DeviceType)vdms_sb_dsp_devOutType) + _T(".WaveOut\n");
      vdmsini += _T("[SBWavePlayer.debug]\n");
      vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logsblaster) ? vdms_debug_logdetail : log_none) + _T("\n");
      vdmsini += _T("[SBWavePlayer.config]\n");
      vdmsini += _T("device=") + VLPUtil::FormatString(_T("%d"), vdms_sb_dsp_devOutID) + _T("\n");
      vdmsini += _T("buffer=") + VLPUtil::FormatString(_T("%d"), vdms_sb_dsp_buffer) + _T("\n");
    }
  }

  //
  // SB - FM (AdLib)
  //
  if (vdms_sb_fm_enabled) {
    vdmsini += _T("[AdLibController]\n");
    vdmsini += _T("CLSID=EmuAdLib.AdLibCtl\n");
    vdmsini += _T("[AdLibController.debug]\n");
    vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logadlib) ? vdms_debug_logdetail : log_none) + _T("\n");
    vdmsini += _T("[AdLibController.config]\n");
    vdmsini += _T("port=") + VLPUtil::FormatString(_T("0x%x"), vdms_sb_fm_port) + _T("\n");
    vdmsini += _T("sampleRate=") + VLPUtil::FormatString(_T("%d"), vdms_sb_fm_sampleRate) + _T("\n");
    vdmsini += _T("[AdLibController.depends]\n");
    vdmsini += _T("VDMSrv=VDMServicesProvider\n");

    if (vdms_sb_fm_useDevOut) {
      vdmsini += _T("WaveOut=AdLibWavePlayer\n");

      vdmsini += _T("[AdLibWavePlayer]\n");
      vdmsini += _T("CLSID=") + GetDeviceID((DeviceUtil::DeviceType)vdms_sb_fm_devOutType) + _T(".WaveOut\n");
      vdmsini += _T("[AdLibWavePlayer.debug]\n");
      vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logadlib) ? vdms_debug_logdetail : log_none) + _T("\n");
      vdmsini += _T("[AdLibWavePlayer.config]\n");
      vdmsini += _T("device=") + VLPUtil::FormatString(_T("%d"), vdms_sb_fm_devOutID) + _T("\n");
      vdmsini += _T("buffer=") + VLPUtil::FormatString(_T("%d"), vdms_sb_fm_buffer) + _T("\n");
    }
  }

  //
  // Joystick
  //
  if (vdms_gameport_enabled) {
    vdmsini += _T("[JoystickController]\n");
    vdmsini += _T("CLSID=EmuJoystick.JoystickCtl\n");
    vdmsini += _T("[JoystickController.debug]\n");
    vdmsini += _T("detail=") + VLPUtil::FormatString(_T("%d"), (vdms_debug_logenabled && vdms_debug_logjoystick) ? vdms_debug_logdetail : log_none) + _T("\n");
    vdmsini += _T("[JoystickController.config]\n");
    vdmsini += _T("port=") + VLPUtil::FormatString(_T("0x%x"), vdms_gameport_port) + _T("\n");
    vdmsini += _T("minCoord=") + VLPUtil::FormatString(_T("%d"), vdms_gameport_minCoord) + _T("\n");
    vdmsini += _T("maxCoord=") + VLPUtil::FormatString(_T("%d"), vdms_gameport_maxCoord) + _T("\n");
    vdmsini += _T("pollPeriod=") + VLPUtil::FormatString(_T("%d"), vdms_gameport_pollPeriod) + _T("\n");
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

  CString program_workdir    = VLPUtil::GetExpandedPath(SettingGetString(_T("program"), _T("workdir")));
  CString program_executable = VLPUtil::GetExpandedPath(SettingGetString(_T("program"), _T("executable")));
  CString program_params     = SettingGetString(_T("program"), _T("params"));

  program_executable = VLPUtil::GetAbsolutePath(program_executable, program_workdir);

  // Check that directory exists, that file exists, and that file is valid
  if (!VLPUtil::FileExists(program_workdir)) {
    strTmp.FormatMessage(IDS_MSG_NODIRERR, (LPCTSTR)program_workdir);
    if (MessageBox(NULL, strTmp, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
      return FALSE;
  } else if (!VLPUtil::FileExists(program_executable)) {
    strTmp.FormatMessage(IDS_MSG_NOFILEERR, (LPCTSTR)program_executable);
    if (MessageBox(NULL, strTmp, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
      return FALSE;
  } else if (!VLPUtil::isMSDOSFile(program_executable)) {
    strTmp.FormatMessage(IDS_MSG_NOEXECOMBATERR, (LPCTSTR)program_executable);
    if (MessageBox(NULL, strTmp, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
      return FALSE;
  }

  CString dosdrvPath = VLPUtil::GetShortPath(VLPUtil::GetVDMSFilePath(_T("dosdrv.exe")));

  // Check that VDMSound is installed properly
  if (!VLPUtil::FileExists(dosdrvPath)) {
    strTmp.FormatMessage(IDS_MSG_NOVDMSOUND, (LPCTSTR)dosdrvPath);
    if (MessageBox(NULL, strTmp, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
      return FALSE;
  }

  //
  // CONFIG
  //
  CString config;

  BOOL vdms_winnt_dos_useHIMEM  = SettingGetBool(_T("winnt.dos"),    _T("useHIMEM.SYS"), TRUE);
  BOOL vdms_winnt_dos_useUMB    = SettingGetBool(_T("winnt.dos"),    _T("useUMB"),       TRUE);
  BOOL vdms_winnt_memory_useEMS = SettingGetBool(_T("winnt.memory"), _T("useEMS"),       TRUE);

  strTmp.Format(_T("DOS=%s,%s\r\n"), vdms_winnt_dos_useHIMEM ? _T("HIGH") : _T("LOW"),
                                     vdms_winnt_dos_useUMB   ? _T("UMB")  : _T("NOUMB"));
  config += strTmp;

  if (vdms_winnt_dos_useUMB && vdms_winnt_memory_useEMS) {
    config += _T("EMM=RAM\r\n");
  }

  if (vdms_winnt_dos_useHIMEM) {
    config += _T("DEVICE=%SYSTEMROOT%\\SYSTEM32\\HIMEM.SYS\r\n");
  }

  BOOL vdms_winnt_dos_useCONFIG = SettingGetBool(_T("winnt.dos"),    _T("useConfig"),    TRUE);

  if (vdms_winnt_dos_useCONFIG) {
    CString vdms_winnt_dos_Config = SettingGetString(_T("winnt.dos"), _T("config"), _T("FILES=40"));
    config += vdms_winnt_dos_Config + _T("\r\n");
  }

  //
  // AUTOEXEC
  //
  CString autoexec;

  BOOL vdms_winnt_storage_useCDROM   = SettingGetBool(_T("winnt.storage"), _T("useCDROM"),   FALSE);
  BOOL vdms_winnt_storage_useNetware = SettingGetBool(_T("winnt.storage"), _T("useNetware"), FALSE);
  BOOL vdms_winnt_pmode_useDPMI      = SettingGetBool(_T("winnt.pmode"),   _T("useDPMI"),    TRUE);

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

  BOOL vdms_winnt_dos_useAUTOEXEC    = SettingGetBool(_T("winnt.dos"),    _T("useAutoexec"), TRUE);

  if (vdms_winnt_dos_useAUTOEXEC) {
    CString vdms_winnt_dos_Autoexec = SettingGetString(_T("winnt.dos"), _T("autoexec"), _T(""));
    autoexec += vdms_winnt_dos_Autoexec + _T("\r\n");
  }

  autoexec += dosdrvPath + _T(" \"-i:") + INIFile.GetFileName() + _T("\"\r\n");

  //
  // PIF
  //
  PIFFile.SetWindowTitle(VLPUtil::FormatString(_T("VDMS Launchpad - %s"), m_displayName));

  // Program
  PIFFile.SetProgram(program_executable, program_params);
  PIFFile.SetWorkDir(program_workdir);

  CString iconPath;
  int iconIndex;

  VLPUtil::ParseIconLocation(SettingGetString(_T("program"), _T("icon"), _T("%COMPSEC%")), iconPath, iconIndex);

  PIFFile.SetIcon(iconPath, iconIndex);
  PIFFile.SetCloseOnExit(SettingGetBool(_T("winnt.dosbox"), _T("exitClose"), FALSE));
  PIFFile.SetWarnOnClose(SettingGetBool(_T("winnt.dosbox"), _T("exitWarn"), TRUE));

  PIFFile.SetFastPaste(SettingGetBool(_T("winnt.dosbox"), _T("fastPaste"), TRUE));

  // Memory
  PIFFile.SetMemory(
    SettingGetBool(_T("winnt.memory"), _T("useEMS"), TRUE) ? SettingGetLong(_T("winnt.memory"), _T("memEMS"), 4096)  : 0,
    SettingGetBool(_T("winnt.memory"), _T("useXMS"), TRUE) ? SettingGetLong(_T("winnt.memory"), _T("memXMS"), 16384) : 0);

  // Screen
  PIFFile.SetFullScreen(SettingGetBool(_T("winnt.video"), _T("useVESA"), FALSE));

  // Keys
  PIFFile.SetWinKeys(
    SettingGetBool(_T("winnt.keys"), _T("altTab"),   TRUE),
    SettingGetBool(_T("winnt.keys"), _T("altEsc"),   TRUE),
    SettingGetBool(_T("winnt.keys"), _T("ctrlEsc"),  TRUE),
    SettingGetBool(_T("winnt.keys"), _T("prtSc"),    TRUE),
    SettingGetBool(_T("winnt.keys"), _T("altPrtSc"), TRUE),
    SettingGetBool(_T("winnt.keys"), _T("altEnter"), TRUE),
    SettingGetBool(_T("winnt.keys"), _T("altSpace"), TRUE)
  );

  // Scheduling
  PIFFile.SetScheduler(
    SettingGetBool(_T("winnt.scheduling"), _T("compatHWEmu"), FALSE),
    SettingGetBool(_T("winnt.scheduling"), _T("detectIdle"),  FALSE),
    SettingGetLong(_T("winnt.scheduling"), _T("idlePrio"),    100));

  return PIFFile.Create(config, autoexec);
}

//
//
//
BOOL CRUNWITHVDMSThread::SetupEnv(LPTSTR lpEnvBlock, int ncch) {
  CDOSEnv DOSEnv;

#if 0
  // Bring in the master environment
  DOSEnv.SetEnvBlock();

#else
  LPCTSTR szCOMSPEC    = _tgetenv(_T("COMSPEC"));
  LPCTSTR szTEMP       = _tgetenv(_T("TEMP"));
  LPCTSTR szTMP        = _tgetenv(_T("TMP"));
  LPCTSTR szSYSTEMROOT = _tgetenv(_T("SYSTEMROOT"));
  LPCTSTR szVDMSPath   = _tgetenv(_T("VDMSPATH"));

  ASSERT(szCOMSPEC    != NULL);
  ASSERT(szTEMP       != NULL);
  ASSERT(szTMP        != NULL);
  ASSERT(szSYSTEMROOT != NULL);
  ASSERT(szVDMSPath   != NULL);

  // Bring in some important variables from the master environment
  DOSEnv.SetAt(_T("COMSPEC"),    szCOMSPEC);
  DOSEnv.SetAt(_T("TEMP"),       szTEMP);
  DOSEnv.SetAt(_T("TMP"),        szTMP);
  DOSEnv.SetAt(_T("SYSTEMROOT"), szSYSTEMROOT);

  // Set some other variables
  DOSEnv.SetAt(_T("PATH"), VLPUtil::FormatString(_T("%s;%s"), szCOMSPEC ? VLPUtil::GetDirectory(szCOMSPEC) : _T(""), szVDMSPath));
  DOSEnv.SetAt(_T("PATHEXT"), _T(".COM;.EXE;.BAT"));
  DOSEnv.SetAt(_T("PROMPT"), _T("$P$G"));

#endif

  // Set the BLASTER variable
  CString strBLASTER(_T(""));

  BOOL    vdms_midi_enabled         = SettingGetBool  (_T("vdms.midi"),     _T("enabled"),      TRUE);
  LONG    vdms_midi_port            = SettingGetLong  (_T("vdms.midi"),     _T("port"),         0x330);

  BOOL    vdms_sb_dsp_enabled       = SettingGetBool  (_T("vdms.sb.dsp"),   _T("enabled"),      TRUE);
  LONG    vdms_sb_dsp_port          = SettingGetLong  (_T("vdms.sb.dsp"),   _T("port"),         0x220);
  LONG    vdms_sb_dsp_IRQ           = SettingGetLong  (_T("vdms.sb.dsp"),   _T("IRQ"),          7);
  LONG    vdms_sb_dsp_DMA8          = SettingGetLong  (_T("vdms.sb.dsp"),   _T("DMA8"),         1);
  LONG    vdms_sb_dsp_DMA16         = SettingGetLong  (_T("vdms.sb.dsp"),   _T("DMA16"),        5);
  CString vdms_sb_dsp_version       = SettingGetString(_T("vdms.sb.dsp"),   _T("version"),      _T("4.05 (SoundBlaster 16)"));

  int verMaj, verMin, sbType;
  short DSPVersion;

  if (_stscanf(vdms_sb_dsp_version, _T("%d.%d"), &verMaj, &verMin) != 2) {
    DSPVersion = 0x0405; // default version (4.05)
  } else {
    DSPVersion = MAKEWORD((BYTE)verMin, (BYTE)verMaj);
  }

  if (DSPVersion < 0x0100) {
    sbType = -1;  // invalid
  } else if (DSPVersion < 0x0200) {
    sbType = 1;   // SB 1.x
  } else if (DSPVersion < 0x0300) {
    sbType = 3;   // SB 2.x
  } else if (DSPVersion < 0x0302) {
    sbType = 2;   // SBPro
  } else if (DSPVersion < 0x0400) {
    sbType = 4;   // SBPro 2
  } else {
    sbType = 6;   // SB16, AWE32
  }

  if (vdms_sb_dsp_enabled) {
    strBLASTER += VLPUtil::FormatString(_T("A%03x I%d D%d"), vdms_sb_dsp_port, vdms_sb_dsp_IRQ, vdms_sb_dsp_DMA8);
  }

  if (vdms_sb_dsp_enabled && (DSPVersion >= 0x0400)) {
    strBLASTER += VLPUtil::FormatString(_T(" H%d"), vdms_sb_dsp_DMA16);
  }

  if (vdms_midi_enabled) {
    strBLASTER += VLPUtil::FormatString(_T(" P%03x"), vdms_midi_port);
  }

  if (vdms_sb_dsp_enabled && (sbType != -1)) {
    strBLASTER += VLPUtil::FormatString(_T(" T%d"), sbType);
  }

  strBLASTER.TrimLeft();
  strBLASTER.TrimRight();

  if (!strBLASTER.IsEmpty()) {
    DOSEnv.SetAt(_T("BLASTER"), strBLASTER);
  }

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
BOOL CRUNWITHVDMSThread::SettingGetBool(LPCTSTR section, LPCTSTR key, BOOL defValue) {
  CString strDefValue(defValue ? VLPUtil::T_YES : VLPUtil::T_NO);
  CString strRetVal(strDefValue);

  m_settings.GetValue(section, key, strRetVal, NULL, strDefValue);
  strRetVal.TrimLeft(); strRetVal.TrimRight();

  if (strRetVal.CompareNoCase(VLPUtil::T_YES) == 0) {
    return TRUE;
  } else if (strRetVal.CompareNoCase(VLPUtil::T_NO) == 0) {
    return FALSE;
  } else {
    return defValue;
  }
}

//
//
//
LONG CRUNWITHVDMSThread::SettingGetLong(LPCTSTR section, LPCTSTR key, LONG defValue) {
  LONG retVal;
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

CString CRUNWITHVDMSThread::GetDeviceID(DeviceUtil::DeviceType devType) {
  switch (devType) {
    case DeviceUtil::DEV_MIDI:
      return _T("MIDIDevice");
    case DeviceUtil::DEV_WAVE:
      return _T("WaveDevice");
    case DeviceUtil::DEV_DSOUND:
      return _T("DSoundDevice");
    default:
      return _T("<unknown>");
  }
}
