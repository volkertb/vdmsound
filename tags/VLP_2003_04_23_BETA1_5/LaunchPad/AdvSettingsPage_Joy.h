#if !defined(AFX_ADVSETTINGSPAGE_JOY_H__73CFF61B_237C_4E23_A484_A42F049C42B0__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_JOY_H__73CFF61B_237C_4E23_A484_A42F049C42B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Joy.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Joy dialog

class CAdvSettingsPage_Joy : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Joy(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Joy();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Joy)
	enum { IDD = IDD_ADVPROPPAGE_JOY };
	CEdit	m_edtJoymapbrowse;
	CSpinButtonCtrl	m_spnJoyscalemin;
	CSpinButtonCtrl	m_spnJoyscalemax;
	CComboBox	m_cmbJoyscalemin;
	CComboBox	m_cmbJoyscalemax;
	CComboBox	m_cmbJoyport;
	CComboBox	m_cmbJoypoll;
	CButton	m_chkUsejoy;
	CButton	m_butJoymapbrowse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Joy)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Joy)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUsejoy();
	afx_msg void OnButJoymapbrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_JOY_H__73CFF61B_237C_4E23_A484_A42F049C42B0__INCLUDED_)
