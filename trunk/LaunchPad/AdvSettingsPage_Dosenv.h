#if !defined(AFX_ADVSETTINGSPAGE_DOSENV_H__36474757_1C4A_44F6_A991_C67A5242FF22__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_DOSENV_H__36474757_1C4A_44F6_A991_C67A5242FF22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Dosenv.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Dosenv dialog

class CAdvSettingsPage_Dosenv : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Dosenv(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Dosenv();

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
	//{{AFX_DATA(CAdvSettingsPage_Dosenv)
	enum { IDD = IDD_ADVPROPPAGE_DOSENV };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Dosenv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Dosenv)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_DOSENV_H__36474757_1C4A_44F6_A991_C67A5242FF22__INCLUDED_)
