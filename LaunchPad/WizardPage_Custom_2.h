#if !defined(AFX_WIZARDPAGE_CUSTOM_2_H__3AD0D460_5430_49F5_8FC0_40281FFB3042__INCLUDED_)
#define AFX_WIZARDPAGE_CUSTOM_2_H__3AD0D460_5430_49F5_8FC0_40281FFB3042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardPage_Custom_2.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////

struct WizardContainer;

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_2 dialog

class CWizardPage_Custom_2 : public CPropertyPageEx
{
// Construction
public:
	CWizardPage_Custom_2(WizardContainer& wizard);
	~CWizardPage_Custom_2();

// Member variables
protected:
  WizardContainer& m_wizard;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CWizardPage_Custom_2)
	enum { IDD = IDD_WIZPROPPAGE_CUSTOM_2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPage_Custom_2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizardPage_Custom_2)
	virtual BOOL OnInitDialog();
	afx_msg void OnButAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPAGE_CUSTOM_2_H__3AD0D460_5430_49F5_8FC0_40281FFB3042__INCLUDED_)
