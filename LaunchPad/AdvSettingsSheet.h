#if !defined(AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_)
#define AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#define WM_SAVESETTINGS   (WM_APP + 0x100)

/////////////////////////////////////////////////////////////////////////////

#include "ContextHelp.h"

#include "AdvSettingsPage_Program.h"
#include "AdvSettingsPage_Compat.h"
#include "AdvSettingsPage_Dosenv.h"
#include "AdvSettingsPage_Perf.h"
#include "AdvSettingsPage_Midi.h"
#include "AdvSettingsPage_Adlib.h"
#include "AdvSettingsPage_Sb.h"
#include "AdvSettingsPage_Joy.h"
#include "AdvSettingsPage_Debug.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsSheet

class CAdvSettingsSheet : public CPropertySheet
{
// Construction
public:
	CAdvSettingsSheet(CLaunchPadSettings& settings, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAdvSettingsSheet(CLaunchPadSettings& settings, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Utility
protected:
  void Init(void);

// Member variables
protected:
  CContextHelp m_help;
//COleDropTarget m_dropTarget;
class CBLA : public COleDropTarget  {
DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) {
return DROPEFFECT_COPY;
}
DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) {
return DROPEFFECT_COPY;
}
BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) {
pWnd->SetWindowText(_T("BLAAAAA"));
return FALSE;
}
} m_dropTarget;

  // Must be declared (constructed) before m_p*
  CLaunchPadSettings& m_settingsOriginal; // reference to original settings
  CLaunchPadSettings  m_settings;         // working copy of settings, to be transferred to original settings when approved

  // Must be declared (constructed) after m_settings*
  CAdvSettingsPage_Program m_p1;
  CAdvSettingsPage_Compat  m_p2;
  CAdvSettingsPage_Dosenv  m_p3;
  CAdvSettingsPage_Perf    m_p4;
  CAdvSettingsPage_Midi    m_p5;
  CAdvSettingsPage_Adlib   m_p6;
  CAdvSettingsPage_Sb      m_p7;
  CAdvSettingsPage_Joy     m_p8;
  CAdvSettingsPage_Debug   m_p9;

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
	afx_msg LRESULT OnSaveSettings(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSSHEET_H__8D31A630_BAC0_4990_B8CA_FF63146541DC__INCLUDED_)
