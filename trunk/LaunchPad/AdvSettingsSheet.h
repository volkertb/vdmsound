#if !defined(AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_)
#define AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "ContextHelp.h"

#include "AdvSettingsPage_Program.h"
#include "AdvSettingsPage_Compat.h"
#include "AdvSettingsPage_Dosenv.h"
#include "AdvSettingsPage_Midi.h"
#include "AdvSettingsPage_Adlib.h"
#include "AdvSettingsPage_Sb.h"
#include "AdvSettingsPage_Joy.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsSheet

class CAdvSettingsSheet : public CPropertySheet
{
// Construction
public:
	CAdvSettingsSheet(CLaunchPadSettings& settings, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAdvSettingsSheet(CLaunchPadSettings& settings, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

protected:
  void Init(void);

// Member variables
protected:
  CContextHelp m_help;

  CAdvSettingsPage_Program m_p1;
  CAdvSettingsPage_Compat  m_p2;
  CAdvSettingsPage_Dosenv  m_p3;
  CAdvSettingsPage_Midi    m_p4;
  CAdvSettingsPage_Adlib   m_p5;
  CAdvSettingsPage_Sb      m_p6;
  CAdvSettingsPage_Joy     m_p7;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAdvSettingsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAdvSettingsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_)
