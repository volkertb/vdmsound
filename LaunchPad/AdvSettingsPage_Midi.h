#if !defined(AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Midi.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

#include "DeviceUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi dialog

class CAdvSettingsPage_Midi : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Midi(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Midi();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_SysEx(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_DevOut(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_FileOut(BOOL bSave, BOOL bEnabled = TRUE);
  BOOL SyncGUIData_Enabled_DevIn(BOOL bSave, BOOL bEnabled = TRUE);

  VOID InitDeviceList(void);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;
  DeviceUtil::DeviceInfoList m_devOutInfo;
  DeviceUtil::DeviceInfoList m_devInInfo;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Midi)
	enum { IDD = IDD_ADVPROPPAGE_MIDI };
	CButton	m_butMpuoutfilebrowse;
	CDDEdit	m_edtMpuoutfile;
	CComboBox	m_cmbMpuindev;
	CButton	m_chkMpuoutfile;
	CButton	m_chkMpuindev;
	CButton	m_chkSysexindicator;
	CEdit	m_edtMidimapbrowse;
	CComboBox	m_cmbSysexindicator;
	CComboBox	m_cmbMpuport;
	CComboBox	m_cmbMpuoutdev;
	CComboBox	m_cmbMpuirq;
	CButton	m_chkUsempu;
	CButton	m_chkMpuoutdev;
	CButton	m_butMidimapbrowse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Midi)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Midi)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUsempu();
	afx_msg void OnChkMpuoutdev();
	afx_msg void OnButMidimapbrowse();
	afx_msg void OnChkSysexindicator();
	afx_msg void OnChkMpuindev();
	afx_msg void OnChkMpuoutfile();
	afx_msg void OnButMpuoutfilebrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_)
