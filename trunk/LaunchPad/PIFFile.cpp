// PIFFile.cpp: implementation of the CPIFFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "launchpad.h"
#include "PIFFile.h"

#include <memory>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define _LENOF(str) (sizeof(str) / sizeof(str[0]))

#define _CHANGEBIT(val, cond, mask) ((val) = ((cond) ? ((val) | (mask)) : ((val) & (~mask))))

//////////////////////////////////////////////////////////////////////

static LPCTSTR VLP_PREFIX    = _T("~VLP");
static LPCTSTR PIF_EXTENSION = _T("PIF");
static LPCTSTR NT_EXTENSION  = _T("NT");
static LPCTSTR INI_EXTENSION = _T("INI");

//////////////////////////////////////////////////////////////////////
//
// CTempFile
//
//////////////////////////////////////////////////////////////////////

#define   MAX_RETRIES 16      // how many times to retry creation of a file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempFile::CTempFile() :
  m_bExists(FALSE),
  m_hFile(INVALID_HANDLE_VALUE),
  m_strFileName(_T(""))
{
}

CTempFile::~CTempFile()
{
  if (m_bExists) {
    Delete();
  }
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
// Creates a temporary file with the given prefix, extension and
//  attributes
//
BOOL CTempFile::Create(
  LPCTSTR szPrefix,
  LPCTSTR szExtension,
  DWORD dwDesiredAccess,
  DWORD dwShareMode,
  DWORD dwFlagsAndAttributes)
{
  ASSERT(!m_bExists);
  ASSERT(m_hFile == INVALID_HANDLE_VALUE);

  if (m_bExists)
    return FALSE;

  TCHAR szTmpPath[MAX_PATH + 1];

  if (GetTempPath(_LENOF(szTmpPath), szTmpPath) < 1)
    return FALSE;           // could not obtain temporary folder's path

  srand((unsigned)time(NULL));

  // Try to generate a unique file name within MAX_RETRIES tries
  for (int i = 0; i < MAX_RETRIES; i++) {
    m_strFileName.Format(_T("%s%s%04X.%s"), szTmpPath, szPrefix, rand(), szExtension);

    if ((m_hFile = CreateFile(m_strFileName, dwDesiredAccess, dwShareMode, NULL, CREATE_NEW, dwFlagsAndAttributes, NULL)) != INVALID_HANDLE_VALUE)
      break;
  }

  return (m_bExists = (m_hFile != INVALID_HANDLE_VALUE));
}

//
// Truncates the file to the desired size
//
BOOL CTempFile::Truncate(
  DWORD newSize)
{
  ASSERT(m_bExists);
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  if (SetFilePointer(m_hFile, newSize, NULL, FILE_BEGIN) == (DWORD)(-1))
    return FALSE;

  return SetEndOfFile(m_hFile);
}

//
// Writes raw data to the temporary file
//
DWORD CTempFile::Write(
  LPCVOID lpBuffer,
  DWORD nNumberOfBytesToWrite)
{
  ASSERT(m_bExists);
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  DWORD numberOfBytesWritten;

  if (!WriteFile(m_hFile, lpBuffer, nNumberOfBytesToWrite, &numberOfBytesWritten, NULL))
    return 0;

  return numberOfBytesWritten;
}

//
// Closes the temporary file
//
BOOL CTempFile::Close(void) {
  ASSERT(m_bExists);
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  if (CloseHandle(m_hFile)) {
    m_hFile = INVALID_HANDLE_VALUE;
    return TRUE;
  } else {
    return FALSE;
  }
}

//
// Deletes the temporary file from disk
//
BOOL CTempFile::Delete(void) {
  ASSERT(m_bExists);

  if (!m_bExists)
    return FALSE;

  if (m_hFile != INVALID_HANDLE_VALUE)
    Close();

  if (DeleteFile(m_strFileName)) {
    m_bExists = FALSE;
    return TRUE;
  } else {
    return FALSE;
  }
}

//
// Returns whether the temporary file exists on disk or not
//
BOOL CTempFile::Exists(void) {
  return m_bExists;
}

//
// Returns the name of the temporary file
//
CString CTempFile::GetFileName(void) {
  ASSERT(m_bExists);

  if (!m_bExists)
    return _T("");

  return m_strFileName;
}



//////////////////////////////////////////////////////////////////////
//
// CINIFile
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINIFile::CINIFile()
{
}

CINIFile::~CINIFile()
{
  Delete();
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
//
//
BOOL CINIFile::Create(
  LPCTSTR vdmsini)
{
  Delete();

  if (vdmsini != NULL) {
    CArray<CHAR,CHAR> szVdmsIniBuf;
    szVdmsIniBuf.SetSize(_tcslen(vdmsini));

    CharToOemBuff(vdmsini, szVdmsIniBuf.GetData(), szVdmsIniBuf.GetSize());

    if (!m_INIFile.Create(VLP_PREFIX, INI_EXTENSION) ||
        !m_INIFile.Write(szVdmsIniBuf.GetData(), szVdmsIniBuf.GetSize()) ||
        !m_INIFile.Close())
    {
      return FALSE;
    }
  }

  return TRUE;
}

//
//
//
BOOL CINIFile::Delete(void) {
  if (m_INIFile.Exists()) {
    return m_INIFile.Delete();
  } else {
    return TRUE;
  }
}

//
//
//
CString CINIFile::GetFileName(void) {
  if (!m_INIFile.Exists())
    return _T("");

  return m_INIFile.GetFileName();
}



//////////////////////////////////////////////////////////////////////
//
// CPIFFile
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPIFFile::CPIFFile()
{
  Reset();
}

CPIFFile::~CPIFFile()
{
  Delete();
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
//
//
BOOL CPIFFile::Create(
  LPCTSTR config,
  LPCTSTR autoexec)
{
  Delete();

  if (config != NULL) {
    CArray<CHAR,CHAR> szConfigBuf;
    szConfigBuf.SetSize(_tcslen(config));

    CharToOemBuff(config, szConfigBuf.GetData(), szConfigBuf.GetSize());

    if (!m_configFile.Create(VLP_PREFIX, NT_EXTENSION) ||
        !m_configFile.Write(szConfigBuf.GetData(), szConfigBuf.GetSize()) ||
        !m_configFile.Close())
    {
      return FALSE;
    }

    ToANSI(m_configFile.GetFileName(), m_PIF.winNT3.szConfig, _LENOF(m_PIF.winNT3.szConfig));
  }

  if (autoexec != NULL) {
    CArray<CHAR,CHAR> szAutoexecBuf;
    szAutoexecBuf.SetSize(_tcslen(autoexec));

    CharToOemBuff(autoexec, szAutoexecBuf.GetData(), szAutoexecBuf.GetSize());

    if (!m_autoexecFile.Create(VLP_PREFIX, NT_EXTENSION) ||
        !m_autoexecFile.Write(szAutoexecBuf.GetData(), szAutoexecBuf.GetSize()) ||
        !m_autoexecFile.Close())
    {
      return FALSE;
    }

    ToANSI(m_autoexecFile.GetFileName(), m_PIF.winNT3.szAutoexec, _LENOF(m_PIF.winNT3.szAutoexec));
  }

  if (!m_PIFFile.Create(VLP_PREFIX, PIF_EXTENSION))
    return FALSE;

  if (!m_PIFFile.Write(&m_PIF, sizeof(m_PIF)))
    return FALSE;

  if (!m_PIFFile.Close())
    return FALSE;

  return TRUE;
}

//
//
//
BOOL CPIFFile::Delete(void) {
  BOOL retVal = TRUE;

  if (m_PIFFile.Exists())
    retVal &= m_PIFFile.Delete();

  if (m_autoexecFile.Exists())
    retVal &= m_autoexecFile.Delete();

  if (m_configFile.Exists())
    retVal &= m_configFile.Delete();

  return retVal;
}

//
//
//
CString CPIFFile::GetFileName(void) {
  if (!m_PIFFile.Exists())
    return _T("");

  return m_PIFFile.GetFileName();
}

//
//
//
void CPIFFile::SetWindowTitle(LPCTSTR szTitle) {
  ToOEM(szTitle, m_PIF.basic.szWndTitle, _LENOF(m_PIF.basic.szWndTitle));
  memset(&m_PIF.basic.szWndTitle[strlen(m_PIF.basic.szWndTitle)], 0x20, sizeof(m_PIF.basic.szWndTitle) - strlen(m_PIF.basic.szWndTitle));

  ToANSI(szTitle, m_PIF.winNT4.szWndTitleA, _LENOF(m_PIF.winNT4.szWndTitleA));
  ToUNICODE(szTitle, m_PIF.winNT4.szWndTitleW, _LENOF(m_PIF.winNT4.szWndTitleW));
}

//
//
//
void CPIFFile::SetProgram(LPCTSTR szExecutable, LPCTSTR szArguments) {
  CString strCmdLine;
  strCmdLine.Format(_T("\"%s\" %s"), szExecutable, szArguments);

  ToOEM(szExecutable, m_PIF.basic.szProgFName, _LENOF(m_PIF.basic.szProgFName));
  ToOEM(szArguments, m_PIF.basic.szParams, _LENOF(m_PIF.basic.szParams));
  ToOEM(szArguments, m_PIF.win386.szParams, _LENOF(m_PIF.win386.szParams));

  ToANSI(strCmdLine, m_PIF.winNT4.szCmdLineA, _LENOF(m_PIF.winNT4.szCmdLineA));
  ToUNICODE(strCmdLine, m_PIF.winNT4.szCmdLineW, _LENOF(m_PIF.winNT4.szCmdLineW));
}

//
//
//
void CPIFFile::SetWorkDir(LPCTSTR szWDir) {
  ToOEM(szWDir, m_PIF.basic.szWorkingDir, _LENOF(m_PIF.basic.szWorkingDir));

  ToANSI(szWDir, m_PIF.winNT4.szWorkDirA, _LENOF(m_PIF.winNT4.szWorkDirA));
  ToUNICODE(szWDir, m_PIF.winNT4.szWorkDirW, _LENOF(m_PIF.winNT4.szWorkDirW));
}

//
//
//
void CPIFFile::SetIcon(LPCTSTR szIcon, LONG iconID) {
  m_PIF.winVMM.wIconID = iconID;

  ToANSI(szIcon, m_PIF.winVMM.szIconFile, _LENOF(m_PIF.winVMM.szIconFile));
  ToANSI(szIcon, m_PIF.winNT4.szIconFileA, _LENOF(m_PIF.winNT4.szIconFileA));
  ToUNICODE(szIcon, m_PIF.winNT4.szIconFileW, _LENOF(m_PIF.winNT4.szIconFileW));
}

//
//
//
void CPIFFile::SetCloseOnExit(BOOL closeOnExit) {
  _CHANGEBIT(m_PIF.basic.wFlags1, closeOnExit, PIF_BAS_CLOSEONEXIT);
}

//
//
//
void CPIFFile::SetMemory(LONG EMS, LONG XMS) {
  m_PIF.win386.wMaxEMSMem  = EMS;
  m_PIF.win386.wReqEMSMem  = EMS;

  m_PIF.win386.wMaxXMSMem  = XMS;
  m_PIF.win386.wReqXMSMem  = XMS;
}

//
//
//
void CPIFFile::SetFullScreen(BOOL fullScreen) {
  _CHANGEBIT(m_PIF.win386.dwFlags1, fullScreen, PIF_386_FULLSCREEN);
  _CHANGEBIT(m_PIF.winVMM.wFlags2,  fullScreen, PIF_VMM_FULLSCREEN);
}

//
//
//
void CPIFFile::SetFastPaste(BOOL fastPaste) {
  _CHANGEBIT(m_PIF.win386.dwFlags1, fastPaste, PIF_386_FASTPASTE);
  _CHANGEBIT(m_PIF.winVMM.wFlags3,  fastPaste, PIF_VMM_FASTPASTE);
}

//
//
//
void CPIFFile::SetWarnOnClose(BOOL warnOnClose) {
  _CHANGEBIT(m_PIF.win386.dwFlags1, !warnOnClose, PIF_386_ALLOWCLOSE);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !warnOnClose, PIF_VMM_NOWARNONEXIT);
}

//
//
//
void CPIFFile::SetWinKeys(BOOL useAltTab, BOOL useAltEsc, BOOL useCtrlEsc, BOOL usePrtSc, BOOL useAltPrtSc, BOOL useAltEnter, BOOL useAltSpace) {
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useAltTab,   PIF_386_NOALTTAB);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useAltEsc,   PIF_386_NOALTESC);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useCtrlEsc,  PIF_386_NOCTRLESC);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !usePrtSc,    PIF_386_NOPRTSC);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useAltPrtSc, PIF_386_NOALTPRTSC);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useAltEnter, PIF_386_NOALTENTER);
  _CHANGEBIT(m_PIF.win386.dwFlags1, !useAltSpace, PIF_386_NOALTSPACE);

  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useAltTab,   PIF_VMM_NOALTTAB);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useAltEsc,   PIF_VMM_NOALTESC);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useCtrlEsc,  PIF_VMM_NOCTRLESC);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !usePrtSc,    PIF_VMM_NOPRTSC);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useAltPrtSc, PIF_VMM_NOALTPRTSC);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useAltEnter, PIF_VMM_NOALTENTER);
  _CHANGEBIT(m_PIF.winVMM.wFlags1,  !useAltSpace, PIF_VMM_NOALTSPACE);
}

//
//
//
void CPIFFile::Reset(void) {
  memset(&m_PIF, 0x00, sizeof(m_PIF));

  strcpy(m_PIF.winVMM.szIconFile, "PIFMGR.DLL");
  strcpy(m_PIF.winVMM.szFntName1, "Terminal");
  strcpy(m_PIF.winVMM.szFntName2, "Lucida Console");

  strcpy(m_PIF.winNT3.szConfig, "%SystemRoot%\\SYSTEM32\\CONFIG.NT");
  strcpy(m_PIF.winNT3.szAutoexec, "%SystemRoot%\\SYSTEM32\\AUTOEXEC.NT");

  strcpy(m_PIF.basicHdr.szSectionName,  "MICROSOFT PIFEX");
  strcpy(m_PIF.win386Hdr.szSectionName, "WINDOWS 386 3.0");
  strcpy(m_PIF.winVMMHdr.szSectionName, "WINDOWS VMM 4.0");
  strcpy(m_PIF.winNT3Hdr.szSectionName, "WINDOWS NT  3.1");
  strcpy(m_PIF.winNT4Hdr.szSectionName, "WINDOWS NT  4.0");

  memset(&m_PIF.basic.szWndTitle, 0x20, sizeof(m_PIF.basic.szWndTitle));

  m_PIF.basic.cReserved1               = 0x00;
  m_PIF.basic.cChecksum                = 0xc9;
  m_PIF.basic.wMaxConvMem              = 0x0080;
  m_PIF.basic.wMinConvMem              = 0x0080;
  m_PIF.basic.wFlags1                  = 0x0000;
  m_PIF.basic.cReserved2               = 0x00;
  m_PIF.basic.cReserved3               = 0x01;
  m_PIF.basic.cReserved4               = 0x00;
  m_PIF.basic.cReserved5               = 0xff;
  m_PIF.basic.cReserved6               = 0x19;
  m_PIF.basic.cReserved7               = 0x50;
  m_PIF.basic.cReserved8               = 0x00;
  m_PIF.basic.cReserved9               = 0x00;
  m_PIF.basic.wReserved10              = 0x0007;
  m_PIF.basic.wFlags2                  = 0x20e0;      // ?? Exchange interrupt vectors, Directly modify screen, ?? Stop in background mode

  m_PIF.basicHdr.wNextSecHeaderOffset  = getPIFOffset(&m_PIF.win386Hdr);
  m_PIF.basicHdr.wThisSecBodyOffset    = getPIFOffset(&m_PIF.basic);
  m_PIF.basicHdr.wThisSecBodyLength    = sizeof(m_PIF.basic);

  m_PIF.win386Hdr.wNextSecHeaderOffset = getPIFOffset(&m_PIF.winVMMHdr);
  m_PIF.win386Hdr.wThisSecBodyOffset   = getPIFOffset(&m_PIF.win386);
  m_PIF.win386Hdr.wThisSecBodyLength   = sizeof(m_PIF.win386);

  m_PIF.win386.wMaxConvMem             = 0x0280;
  m_PIF.win386.wReqConvMem             = 0x0000;
  m_PIF.win386.wActivePrio             = 0x0064;
  m_PIF.win386.wBackgroundPrio         = 0x0032;
  m_PIF.win386.wMaxEMSMem              = 0x0000;
  m_PIF.win386.wReqEMSMem              = 0x0000;
  m_PIF.win386.wMaxXMSMem              = 0x0400;
  m_PIF.win386.wReqXMSMem              = 0x0000;
  m_PIF.win386.dwFlags1                = 0x01021000;  // Prevent Windows detection, Use fast paste, Detect idle time
  m_PIF.win386.wFlags2                 = 0x001f;      // Video memory: text, Do not check ports: high graphics, Do not check ports: low graphics, Do not check ports: text, Video ROM emulation
  m_PIF.win386.wReserved1              = 0x0000;
  m_PIF.win386.wShortcutSCode          = 0x0000;
  m_PIF.win386.wShortcutModif          = 0x0000;
  m_PIF.win386.wShortcutEnable         = 0x0000;
  m_PIF.win386.wShortcutIsExtended     = 0x0000;
  m_PIF.win386.wReserved2              = 0x0064;
  m_PIF.win386.wReserved3              = 0x0032;
  m_PIF.win386.dwReserved4             = 0x00000000;

  m_PIF.winVMMHdr.wNextSecHeaderOffset = getPIFOffset(&m_PIF.winNT3Hdr);
  m_PIF.winVMMHdr.wThisSecBodyOffset   = getPIFOffset(&m_PIF.winVMM);
  m_PIF.winVMMHdr.wThisSecBodyLength   = sizeof(m_PIF.winVMM);

  m_PIF.winVMM.wIconID                 = 0x0000;
  m_PIF.winVMM.wFlags1                 = 0x0000;
  m_PIF.winVMM.wPriority               = 0x0032;
  m_PIF.winVMM.wFlags2                 = 0x0001;      // Video-ROM emulation
  m_PIF.winVMM.wTxtRows                = 0x0000;
  m_PIF.winVMM.wFlags3                 = 0x0001;      // Fast paste
  m_PIF.winVMM.wReserved4              = 0x0000;
  m_PIF.winVMM.wReserved5              = 0x0005;
  m_PIF.winVMM.wReserved6              = 0x0019;
  m_PIF.winVMM.wReserved7              = 0x0003;
  m_PIF.winVMM.wReserved8              = 0x00c8;
  m_PIF.winVMM.wReserved9              = 0x03e8;      // ??? VGA ???
  m_PIF.winVMM.wReserved10             = 0x0002;
  m_PIF.winVMM.wReserved11             = 0x000a;
  m_PIF.winVMM.wFlags4                 = 0x0001;      // Not use the mouse for selection
  m_PIF.winVMM.wFlags5                 = 0x001c;      // Use raster fonts, Use TrueType fonts, Automatically choose the font size
  m_PIF.winVMM.wReserved13             = 0x0000;
  m_PIF.winVMM.wFntWidth1              = 0x0000;
  m_PIF.winVMM.wFntHeight1             = 0x0000;
  m_PIF.winVMM.wFntWidth2              = 0x0000;
  m_PIF.winVMM.wFntHeight2             = 0x0000;
  m_PIF.winVMM.wReserved14             = 0x04e4;
  m_PIF.winVMM.wFlags6                 = 0x0003;      // Unknown, Show toolbar
  m_PIF.winVMM.wNoSettingsRestore      = 0x0000;
  m_PIF.winVMM.wWndColumns             = 0x0000;
  m_PIF.winVMM.wWndRows                = 0x0000;
  m_PIF.winVMM.wWndWidth               = 0x0000;
  m_PIF.winVMM.wWndHeight              = 0x0000;
  m_PIF.winVMM.wWndNCWidth             = 0x0000;
  m_PIF.winVMM.wWndNCHeight            = 0x0000;
  m_PIF.winVMM.wReserved15             = 0x0016;
  m_PIF.winVMM.wWndState1              = 0x0000;
  m_PIF.winVMM.wWndState2              = 0x0000;
  m_PIF.winVMM.wReserved16             = 0x0000;
  m_PIF.winVMM.wReserved17             = 0x0000;
  m_PIF.winVMM.wWndMaxRPos             = 0x0000;
  m_PIF.winVMM.wWndMaxBPos             = 0x0000;
  m_PIF.winVMM.wWndLPos                = 0x0000;
  m_PIF.winVMM.wWndTPos                = 0x0000;
  m_PIF.winVMM.wWndNormRPos            = 0x0000;
  m_PIF.winVMM.wWndNormBPos            = 0x0000;
  m_PIF.winVMM.wEnvMem                 = 0x0000;
  m_PIF.winVMM.wDPMIMem                = 0x0000;
  m_PIF.winVMM.wReserved19             = 0x0001;

  m_PIF.winNT3Hdr.wNextSecHeaderOffset = getPIFOffset(&m_PIF.winNT4Hdr);
  m_PIF.winNT3Hdr.wThisSecBodyOffset   = getPIFOffset(&m_PIF.winNT3);
  m_PIF.winNT3Hdr.wThisSecBodyLength   = sizeof(m_PIF.winNT3);

  m_PIF.winNT3.wFlags                  = 0x0000;
  m_PIF.winNT3.wReserved2              = 0x0000;

  m_PIF.winNT4Hdr.wNextSecHeaderOffset = 0xffff;
  m_PIF.winNT4Hdr.wThisSecBodyOffset   = getPIFOffset(&m_PIF.winNT4);
  m_PIF.winNT4Hdr.wThisSecBodyLength   = sizeof(m_PIF.winNT4);
}


/////////////////////////////////////////////////


//
//
//
void CPIFFile::ToANSI(LPCTSTR src, LPSTR dst, LONG ncch) {
  ASSERT(src != NULL);
  ASSERT(dst != NULL);
  ASSERT(ncch > 0);

# ifdef _UNICODE
  WideCharToMultiByte(CP_ACP, 0, src, -1, dst, ncch, NULL, NULL);
# else
  strncpy(dst, src, ncch);
# endif

  dst[ncch - 1] = '\0';
}

//
//
//
void CPIFFile::ToOEM(LPCTSTR src, LPSTR dst, LONG ncch) {
  ASSERT(src != NULL);
  ASSERT(dst != NULL);
  ASSERT(ncch > 0);

  CharToOemBuff(src, dst, ncch);

  dst[ncch - 1] = '\0';
}

//
//
//
void CPIFFile::ToUNICODE(LPCTSTR src, LPWSTR dst, LONG ncch) {
  ASSERT(src != NULL);
  ASSERT(dst != NULL);
  ASSERT(ncch > 0);

# ifdef _UNICODE
  wcsncpy(dst, src, ncch);
# else
  MultiByteToWideChar(CP_ACP, 0, src, -1, dst, ncch);
# endif

  dst[ncch - 1] = L'\0';
}
