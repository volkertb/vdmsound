#if !defined(AFX_WIZARDPAGE_INTRO_H__27A42C58_07FB_4705_AFAE_4EA32FEDD5A5__INCLUDED_)
#define AFX_WIZARDPAGE_INTRO_H__27A42C58_07FB_4705_AFAE_4EA32FEDD5A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardPage_Intro.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////

struct WizardContainer;

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Intro dialog

class CWizardPage_Intro : public CPropertyPageEx
{
// Construction
public:
	CWizardPage_Intro(WizardContainer& wizard);
	~CWizardPage_Intro();

// Helper functions
protected:
  void SyncWizButtons(void);

// Member variables
protected:
  CFont m_fntWizardtitle;
  CFont m_fntBoldButtons;
  WizardContainer& m_wizard;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CWizardPage_Intro)
	enum { IDD = IDD_WIZPROPPAGE_INTRO };
	CButton	m_optUsedefault;
  CButton	m_optUsecustom;
	CStatic	m_txtWizardtitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPage_Intro)
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
	//{{AFX_MSG(CWizardPage_Intro)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPAGE_INTRO_H__27A42C58_07FB_4705_AFAE_4EA32FEDD5A5__INCLUDED_)
