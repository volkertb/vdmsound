#if !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
#define AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

class CPIFFile;

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
	CRUNWITHVDMSThread(LPCTSTR fileName);
	~CRUNWITHVDMSThread();

public:
  static HRESULT CreateThread(LPCTSTR fileName);

// Callback functions
protected:
  virtual BOOL InitInstance(void);
  virtual int Run(void);

// Utility functions
protected:
  BOOL SetupPIF(CPIFFile& PIFFile);
  BOOL SetupEnv(LPTSTR lpEnvBlock, int ncch);

  CString SettingGetString(LPCTSTR section, LPCTSTR key, LPCTSTR defValue = _T(""));
  bool SettingGetBool(LPCTSTR section, LPCTSTR key, bool defValue = false);
  int SettingGetInt(LPCTSTR section, LPCTSTR key, int defValue = 0);

// Member variables
protected:
  CLaunchPadSettings m_settings;
};

#endif // !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
