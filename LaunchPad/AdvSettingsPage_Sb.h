#if !defined(AFX_ADVSETTINGSPAGE_SB_H__DFE792BD_1552_4236_82B9_DFF3FB3AAE4D__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_SB_H__DFE792BD_1552_4236_82B9_DFF3FB3AAE4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Sb.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb dialog

class CAdvSettingsPage_Sb : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Sb(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Sb();

// Helper functions
protected:
  VOID SyncGUIData(BOOL bSave = TRUE);
  VOID SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled = TRUE);
  VOID SyncGUIData_Enabled_Device(BOOL bSave, BOOL bEnabled = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Sb)
	enum { IDD = IDD_ADVPROPPAGE_SB };
	CSpinButtonCtrl	m_spnSboutdevbuf;
	CComboBox	m_cmbSbtype;
	CComboBox	m_cmbSbport;
	CComboBox	m_cmbSboutdevbuf;
	CComboBox	m_cmbSboutdev;
	CComboBox	m_cmbSbirq;
	CComboBox	m_cmbSbdma8;
	CComboBox	m_cmbSbdma16;
	CButton	m_chkUsesb;
	CButton	m_chkSboutdev;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Sb)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Sb)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUsesb();
	afx_msg void OnChkSboutdev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_SB_H__DFE792BD_1552_4236_82B9_DFF3FB3AAE4D__INCLUDED_)
