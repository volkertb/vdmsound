#if !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
#define AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

#include "DeviceUtil.h"

class CPIFFile;
class CINIFile;

/////////////////////////////////////////////////////////////////////////////
// CDOSEnv class

class CDOSEnv : public CMapStringToString
{
// Construction
public:
  CDOSEnv();
  virtual ~CDOSEnv();

// Helper methods
public:
  static BOOL AppendEnvBlockEntry(LPTSTR& lpEnvBlock, LPCTSTR szEntry, int& ncch);

// Methods
public:
  BOOL GetEnvBlock(LPTSTR lpEnvBlock, int ncch) const;
  BOOL SetEnvBlock(void);
  BOOL SetEnvBlock(LPCTSTR lpEnvBlock);
};



/////////////////////////////////////////////////////////////////////////////
// CRUNWITHVDMSThread class

class CRUNWITHVDMSThread : protected CWinThread
{
// Construction
private:
	CRUNWITHVDMSThread(LPCTSTR displayName, const CLaunchPadSettings& settings, LPCTSTR exeFileName);
	~CRUNWITHVDMSThread();

public:
  static HRESULT CreateThread(LPCTSTR displayName, const CLaunchPadSettings& settings, LPCTSTR exeFileName = NULL);

// Callback functions
protected:
  virtual BOOL InitInstance(void);
  virtual int Run(void);

// Utility functions
protected:
  BOOL SetupINI(CINIFile& INIFile);
  BOOL SetupPIF(CPIFFile& PIFFile, CINIFile& INIFile);
  BOOL SetupEnv(LPTSTR lpEnvBlock, int ncch);

  CString SettingGetString(LPCTSTR section, LPCTSTR key, LPCTSTR defValue = _T(""));
  BOOL SettingGetBool(LPCTSTR section, LPCTSTR key, BOOL defValue = FALSE);
  LONG SettingGetLong(LPCTSTR section, LPCTSTR key, LONG defValue = 0);

  static CString GetDeviceID(DeviceUtil::DeviceType devType);
  static CString GetOPLType(LPCTSTR oplMode, LPCTSTR sbVersion);

// Member variables
protected:
  CString m_displayName, m_exeFileName;
  CLaunchPadSettings m_settings;
};

#endif // !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
