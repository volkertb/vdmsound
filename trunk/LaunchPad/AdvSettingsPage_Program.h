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
  VOID SyncGUIData(BOOL bSave = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Program)
	enum { IDD = IDD_ADVPROPPAGE_PROGRAM };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Program)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Program)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_PROGRAM_H__AB1C840E_19C8_41A2_BF2C_F2C542537C7C__INCLUDED_)
