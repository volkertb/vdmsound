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
  VOID SyncGUIData(BOOL bSave = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Adlib)
	enum { IDD = IDD_ADVPROPPAGE_ADLIB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Adlib)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Adlib)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_ADLIB_H__0219D062_2887_4CDE_B13D_17DBD7502ECE__INCLUDED_)
