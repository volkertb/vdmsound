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

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Perf)
	enum { IDD = IDD_ADVPROPPAGE_PERF };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_PERF_H__EEA4D190_5DB6_420E_BDB2_CE6A97C7E744__INCLUDED_)
