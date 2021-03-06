#if !defined(AFX_ADVSETTINGSPAGE_PERF_H__EEA4D190_5DB6_420E_BDB2_CE6A97C7E744__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_PERF_H__EEA4D190_5DB6_420E_BDB2_CE6A97C7E744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Perf.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Perf dialog

class CAdvSettingsPage_Perf : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Perf(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Perf();

// Helper functions
protected:
  BOOL SyncGUIData(BOOL bSave = TRUE);
  BOOL SyncGUIData_DetectIdle(BOOL bSave, BOOL bEnabled = TRUE);

  VOID SetSlideState(BOOL isDeterminate);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;
  enum { PRI_UNKNOWN, PRI_DETERMINATE, PRI_INDETERMINATE } m_sliPriorityType;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Perf)
	enum { IDD = IDD_ADVPROPPAGE_PERF };
	CSpinButtonCtrl	m_spnDmapollmin;
	CSpinButtonCtrl	m_spnDmapollmax;
	CComboBox	m_cmbDmapollmin;
	CComboBox	m_cmbDmapollmax;
	CStatic m_txtPrioritymulti;
	CSliderCtrl	m_sliPriority;
	CButton	m_chkDetectidle;
	CButton	m_chkCompattimer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Perf)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Perf)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChkDetectidle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_PERF_H__EEA4D190_5DB6_420E_BDB2_CE6A97C7E744__INCLUDED_)
