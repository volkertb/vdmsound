#if !defined(AFX_ADVSETTINGSPAGE_DEBUG_H__07212CEE_9859_43AC_BC5D_32E8C87B1F24__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_DEBUG_H__07212CEE_9859_43AC_BC5D_32E8C87B1F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Debug.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Debug dialog

class CAdvSettingsPage_Debug : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Debug(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Debug();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_VDMSLOG(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_VDMSINI(BOOL bSave, BOOL bEnabled = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Debug)
	enum { IDD = IDD_ADVPROPPAGE_DEBUG };
	CDDEdit	m_edtVdmsini;
	CComboBox	m_cmbLogdetail;
	CButton	m_chkVdmslog;
	CButton	m_chkVdmsini;
	CButton	m_chkLogsb;
	CButton	m_chkLogother;
	CButton	m_chkLogmidi;
	CButton	m_chkLogjoy;
	CButton	m_chkLogadlib;
	CButton	m_butLogbrowse;
	CEdit m_edtLogbrowse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Debug)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Debug)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkVdmslog();
	afx_msg void OnChkVdmsini();
	afx_msg void OnButLogbrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_DEBUG_H__07212CEE_9859_43AC_BC5D_32E8C87B1F24__INCLUDED_)
