#if !defined(AFX_ADVSETTINGSPAGE_PROGRAM_H__AB1C840E_19C8_41A2_BF2C_F2C542537C7C__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_PROGRAM_H__AB1C840E_19C8_41A2_BF2C_F2C542537C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Program.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Program dialog

class CAdvSettingsPage_Program : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Program(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Program();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  VLPUtil::CComKeepAlive _autocomplete;    // COM must be present during the entire lifespan of any autocomplete controls
  CContextHelp m_help;
	CString	m_edtDosargs_old;
	CString	m_edtDosdir_old;
	CString	m_edtDosprogram_old;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Program)
	enum { IDD = IDD_ADVPROPPAGE_PROGRAM };
	CDDEdit	m_edtDosprogram;
	CDDEdit	m_edtDosdir;
	CDDEdit	m_edtDosargs;
	CButton	m_chkWinkprtsc;
	CButton	m_chkWinkctrlesc;
	CButton	m_chkWinkalttab;
	CButton	m_chkWinkaltspace;
	CButton	m_chkWinkaltprtsc;
	CButton	m_chkWinkaltesc;
	CButton	m_chkWinkaltenter;
	CButton	m_chkWarn;
	CButton	m_chkFastpaste;
	CButton	m_chkClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Program)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Program)
	virtual BOOL OnInitDialog();
	afx_msg void OnButDosprogrambrowse();
	afx_msg void OnButDosdirbrowse();
	afx_msg void OnButChangeicon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_PROGRAM_H__AB1C840E_19C8_41A2_BF2C_F2C542537C7C__INCLUDED_)
