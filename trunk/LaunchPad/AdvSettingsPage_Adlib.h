#if !defined(AFX_ADVSETTINGSPAGE_ADLIB_H__0219D062_2887_4CDE_B13D_17DBD7502ECE__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_ADLIB_H__0219D062_2887_4CDE_B13D_17DBD7502ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Adlib.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

#include "DeviceUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Adlib dialog

class CAdvSettingsPage_Adlib : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Adlib(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Adlib();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_DevOut(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_FileOut(BOOL bSave, BOOL bEnabled = TRUE);

  VOID InitDeviceList(void);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;
  DeviceUtil::DeviceInfoList m_devOutInfo;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Adlib)
	enum { IDD = IDD_ADVPROPPAGE_ADLIB };
	CEdit	m_edtFmoutfile;
	CButton	m_chkFmoutfile;
	CSpinButtonCtrl	m_spnFmoutdevbuf;
	CComboBox	m_cmbOplmode;
	CComboBox	m_cmbFmsynthrate;
	CComboBox	m_cmbFmport;
	CComboBox	m_cmbFmoutdevbuf;
	CComboBox	m_cmbFmoutdev;
	CButton	m_chkUsefm;
	CButton	m_chkFmoutdev;
	CButton	m_butFmoutfilebrowse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Adlib)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Adlib)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUsefm();
	afx_msg void OnChkFmoutdev();
	afx_msg void OnChkFmoutfile();
	afx_msg void OnButFmoutfilebrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_ADLIB_H__0219D062_2887_4CDE_B13D_17DBD7502ECE__INCLUDED_)
