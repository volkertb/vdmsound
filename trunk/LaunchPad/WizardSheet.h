#if !defined(AFX_WIZARDSHEET_H__8BEDCEDE_DB9B_441C_91D0_8A4DC75D8579__INCLUDED_)
#define AFX_WIZARDSHEET_H__8BEDCEDE_DB9B_441C_91D0_8A4DC75D8579__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizardSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "ContextHelp.h"

#include "WizardPage_Intro.h"
#include "WizardPage_Custom_1.h"
#include "WizardPage_Custom_2.h"
#include "WizardPage_Finis.h"

/////////////////////////////////////////////////////////////////////////////
// WizardContainer
struct WizardContainer
{
  WizardContainer(LPCTSTR _exeFileName)
   : exeFileName(_exeFileName), settings(CLaunchPadSettings(VLPUtil::RenameExtension(exeFileName, _T(".vlp")))) { }

  void ResetProgramInfo(void) {
    settings.SetValue(_T("program"), _T("executable"), exeFileName);
    settings.SetValue(_T("program"), _T("params"), _T(""));
    settings.SetValue(_T("program"), _T("workdir"), VLPUtil::GetDirectory(exeFileName));
  }

  CString exeFileName;
  CLaunchPadSettings settings;
  CList<UINT,UINT> history;
};

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet

class CWizardSheet : public CPropertySheetEx
{
// Construction
public:
	CWizardSheet(LPCTSTR exeFileName, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWizardSheet(LPCTSTR exeFileName, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Utility
protected:
  void Init(void);
  static int GetScreenBpp(void);

// Public stuff
public:
  // Must be declared (constructed) before m_p*
  WizardContainer m_wizard;

// Member variables
protected:
  CContextHelp m_help;
  COleDropTarget m_dropTarget;

  // Must be declared (constructed) after m_wizard*
  CWizardPage_Intro     m_p1;
  CWizardPage_Custom_1  m_p2;
  CWizardPage_Custom_2  m_p3;
  CWizardPage_Finis     m_p4;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizardSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWizardSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizardSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZARDSHEET_H__8BEDCEDE_DB9B_441C_91D0_8A4DC75D8579__INCLUDED_)
