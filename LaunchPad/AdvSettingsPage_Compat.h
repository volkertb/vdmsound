#if !defined(AFX_ADVSETTINGSPAGE_COMPAT_H__25121DE4_6411_4092_9810_9129A58998C6__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_COMPAT_H__25121DE4_6411_4092_9810_9129A58998C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Compat.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Compat dialog

class CAdvSettingsPage_Compat : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Compat(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Compat();

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
	//{{AFX_DATA(CAdvSettingsPage_Compat)
	enum { IDD = IDD_ADVPROPPAGE_COMPAT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Compat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Compat)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_COMPAT_H__25121DE4_6411_4092_9810_9129A58998C6__INCLUDED_)
