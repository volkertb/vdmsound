#if !defined(AFX_WIZARDPAGE_FINIS_H__64039447_ABBC_4F6B_A0E8_7D9E0090DD48__INCLUDED_)
#define AFX_WIZARDPAGE_FINIS_H__64039447_ABBC_4F6B_A0E8_7D9E0090DD48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardPage_Finis.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////

struct WizardContainer;

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Finis dialog

class CWizardPage_Finis : public CPropertyPageEx
{
// Construction
public:
	CWizardPage_Finis(WizardContainer& wizard);
	~CWizardPage_Finis();

// Member variables
protected:
  CFont m_fntWizardtitle;
  CFont m_fntBoldButtons;
  WizardContainer& m_wizard;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CWizardPage_Finis)
	enum { IDD = IDD_WIZPROPPAGE_FINIS };
	CButton	m_chkDesktopshort;
	CButton	m_chkSavesettings;
	CStatic	m_txtWizardtitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWizardPage_Finis)
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
	//{{AFX_MSG(CWizardPage_Finis)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDPAGE_FINIS_H__64039447_ABBC_4F6B_A0E8_7D9E0090DD48__INCLUDED_)
