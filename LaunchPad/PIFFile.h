// PIFFile.h: interface for the CPIFFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIFFILE_H__ED1943BD_5299_4EAA_8F32_014CBEBAF250__INCLUDED_)
#define AFX_PIFFILE_H__ED1943BD_5299_4EAA_8F32_014CBEBAF250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PIF_FMT.H"

//
//
//
class CTempFile
{
public:
  CTempFile();
  virtual ~CTempFile();

public:
  BOOL Create(LPCTSTR szPrefix, LPCTSTR szExtension, DWORD dwDesiredAccess = GENERIC_WRITE, DWORD dwShareMode = FILE_SHARE_DELETE, DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL);
  BOOL Truncate(DWORD newSize = 0);
  DWORD Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
  BOOL Close(void);
  BOOL Delete(void);

  BOOL Exists(void);
  CString GetFileName(void);

protected:
  BOOL m_bExists;
  HANDLE m_hFile;
  CString m_strFileName;
};

//
//
//
class CINIFile
{
public:
	CINIFile();
	virtual ~CINIFile();

public:
  BOOL Create(LPCTSTR vdmsini);
  BOOL Delete(void);

  CString GetFileName(void);

protected:
  CTempFile m_INIFile;
};

//
//
//
class CPIFFile
{
public:
	CPIFFile();
	virtual ~CPIFFile();

public:
  BOOL Create(LPCTSTR config = NULL, LPCTSTR autoexec = NULL);
  BOOL Delete(void);

  CString GetFileName(void);

  void SetWindowTitle(LPCTSTR szTitle);
  void SetProgram(LPCTSTR szExecutable, LPCTSTR szArguments);
  void SetWorkDir(LPCTSTR szWDir);
  void SetIcon(LPCTSTR szIcon, int iconID = 0);
  void SetCloseOnExit(bool closeOnExit);
  void SetMemory(int EMS, int XMS);
  void SetFullScreen(bool fullScreen);
  void SetFastPaste(bool fastPaste);
  void SetWarnOnClose(bool warnOnClose);

  void Reset(void);

protected:
  static void ToANSI(LPCTSTR src, LPSTR dst, int ncch);
  static void ToOEM(LPCTSTR src, LPSTR dst, int ncch);
  static void ToUNICODE(LPCTSTR src, LPWSTR dst, int ncch);

protected:
  inline getPIFOffset(const void* ptr)
    { return (const char*)ptr - (const char*)&m_PIF; }

protected:
  CTempFile m_PIFFile, m_configFile, m_autoexecFile;

# pragma pack ( push, 1 )

  struct PIF_STRUC {
    PIF_BASICSECTION      basic;
    PIF_HDRSECTION        basicHdr;
    PIF_HDRSECTION        win386Hdr;
    PIF_WIN386_30_SECTION win386;
    PIF_HDRSECTION        winVMMHdr;
    PIF_VMM_40_SECTION    winVMM;
    PIF_HDRSECTION        winNT3Hdr;
    PIF_NT_31_SECTION     winNT3;
    PIF_HDRSECTION        winNT4Hdr;
    PIF_NT_40_SECTION     winNT4;
  } m_PIF;

# pragma pack ( pop )

};

#endif // !defined(AFX_PIFFILE_H__ED1943BD_5299_4EAA_8F32_014CBEBAF250__INCLUDED_)
