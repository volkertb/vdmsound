#if !defined(AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_)
#define AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicSettingsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage dialog

class CBasicSettingsPage : public CPropertyPage
{
// Construction
private:
	CBasicSettingsPage(const CStringArray& fileNames = CStringArray());
	~CBasicSettingsPage();

public:
  static HRESULT AddPage(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam, const CStringArray& fileNames);

// Callback functions
protected:
  static UINT CALLBACK PropPageCallbackProc(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE pPSP);

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_MIDI(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Joystick(BOOL bSave, BOOL bEnabled = TRUE);

// Dialog Data
	//{{AFX_DATA(CBasicSettingsPage)
	enum { IDD = IDD_BASICPROPPAGE };
	CButton	m_chkUsempu;
	CButton	m_chkUsejoy;
	CStatic_Icon	m_icoApp;
	CEdit	m_edtDoscmd;
	CButton	m_chkEms;
	CButton	m_chkVesa;
	CButton	m_chkCdrom;
	CButton	m_optGmidi;
	CButton	m_optMt32;
	CButton	m_optMidiother;
	CButton	m_optJoy2but;
	CButton	m_optJoy4but;
	CButton	m_optJoyother;
	CButton	m_chkClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBasicSettingsPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBasicSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnButChange();
	afx_msg void OnButAdvanced();
	afx_msg void OnChkUsejoy();
	afx_msg void OnChkUsempu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member variables
protected:
  CLaunchPadSettings m_settings;
  CContextHelp m_help;
  CString m_tmpMIDIStorage, m_tmpJoyStorage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_)
