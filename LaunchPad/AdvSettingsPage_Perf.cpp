// AdvSettingsPage_Perf.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Perf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Perf property page

CAdvSettingsPage_Perf::CAdvSettingsPage_Perf(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Perf::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Perf)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Perf::~CAdvSettingsPage_Perf()
{
}

void CAdvSettingsPage_Perf::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Perf)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Perf, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Perf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Perf::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Perf message handlers

BOOL CAdvSettingsPage_Perf::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Load the information from file into the GUI
  SyncGUIData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Perf::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);
/*
  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_LOGBROWSE) && (nID != IDC_BUT_MPUOUTFILEBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }
*/
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Perf::OnApply() 
{
  CWaitCursor wait;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CWnd* pWndParent = GetParent();

  ASSERT_VALID(pWndParent);

  if (pWndParent != NULL) {
    pWndParent->SendMessage(WM_SAVESETTINGS);
  }

  return CPropertyPage::OnApply();
}
