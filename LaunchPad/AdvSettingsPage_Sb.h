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

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Sb)
	enum { IDD = IDD_ADVPROPPAGE_SB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Sb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Sb)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_SB_H__DFE792BD_1552_4236_82B9_DFF3FB3AAE4D__INCLUDED_)
