#if !defined(AFX_WIZARDPAGE_CUSTOM_1_H__BDBD8146_A121_452A_AFBC_878F5CC0439A__INCLUDED_)
#define AFX_WIZARDPAGE_CUSTOM_1_H__BDBD8146_A121_452A_AFBC_878F5CC0439A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardPage_Custom_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////

struct WizardContainer;

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_1 dialog

class CWizardPage_Custom_1 : public CPropertyPageEx
{
// Construction
public:
	CWizardPage_Custom_1(WizardContainer& wizard);
	~CWizardPage_Custom_1();

// Helper functions
protected:
  void SyncWizButtons(void);

// Member variables
protected:
  CFont m_fntBoldButtons;
  WizardContainer& m_wizard;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CWizardPage_Custom_1)
	enum { IDD = IDD_WIZPROPPAGE_CUSTOM_1 };
	CButton	m_butTemplate;
	CDDEdit	m_edtTemplate;
	CButton	m_optMakenew;
	CButton	m_optUsetemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPage_Custom_1)
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
	//{{AFX_MSG(CWizardPage_Custom_1)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptClicked();
	afx_msg void OnButTemplate();
	afx_msg void OnChangeEdtTemplate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPAGE_CUSTOM_1_H__BDBD8146_A121_452A_AFBC_878F5CC0439A__INCLUDED_)
