// AdvSettingsPage_Dosenv.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Dosenv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Dosenv property page

CAdvSettingsPage_Dosenv::CAdvSettingsPage_Dosenv(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Dosenv::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Dosenv)
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Dosenv::~CAdvSettingsPage_Dosenv()
{
}

void CAdvSettingsPage_Dosenv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Dosenv)
	DDX_Control(pDX, IDC_EDT_CONFIGSYS, m_edtConfigsys);
	DDX_Control(pDX, IDC_EDT_AUTOEXECBAT, m_edtAutoexecbat);
	DDX_Control(pDX, IDC_CHK_HIMEMSYS, m_chkHimemsys);
	DDX_Control(pDX, IDC_CHK_DOSUMB, m_chkDosumb);
	DDX_Control(pDX, IDC_CHK_CONFIGSYS, m_chkConfigsys);
	DDX_Control(pDX, IDC_CHK_AUTOEXECBAT, m_chkAutoexecbat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Dosenv, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Dosenv)
	ON_BN_CLICKED(IDC_CHK_CONFIGSYS, OnChkConfigsys)
	ON_BN_CLICKED(IDC_CHK_AUTOEXECBAT, OnChkAutoexecbat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

VOID CAdvSettingsPage_Dosenv::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  // CONFIG.SYS
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dos"), _T("useHIMEM.SYS"), m_chkHimemsys,    TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dos"), _T("useUMB"),       m_chkDosumb,      TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dos"), _T("useConfig"),    m_chkConfigsys,   TRUE);
  SyncGUIData_CONFIG(bSave, m_chkConfigsys.GetCheck() != BST_UNCHECKED);

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dos"), _T("useAutoexec"),  m_chkAutoexecbat, TRUE);
  SyncGUIData_AUTOEXEC(bSave, m_chkAutoexecbat.GetCheck() != BST_UNCHECKED);
}

VOID CAdvSettingsPage_Dosenv::SyncGUIData_CONFIG(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("winnt.dos"),  _T("config"),      m_edtConfigsys,   _T("FILES=40"));

  if (!bEnabled) {
    m_edtConfigsys.EnableWindow(FALSE);
  }
}

VOID CAdvSettingsPage_Dosenv::SyncGUIData_AUTOEXEC(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("winnt.dos"),  _T("autoexec"),    m_edtAutoexecbat, _T(""));

  if (!bEnabled) {
    m_edtAutoexecbat.EnableWindow(FALSE);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Dosenv message handlers

BOOL CAdvSettingsPage_Dosenv::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Dosenv::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      ((nCode == EN_CHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Dosenv::OnApply() 
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

void CAdvSettingsPage_Dosenv::OnChkConfigsys() 
{
  SyncGUIData_CONFIG(TRUE);  // save current combo-box selection
  SyncGUIData_CONFIG(FALSE, m_chkConfigsys.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Dosenv::OnChkAutoexecbat() 
{
  SyncGUIData_AUTOEXEC(TRUE);  // save current combo-box selection
  SyncGUIData_AUTOEXEC(FALSE, m_chkAutoexecbat.GetCheck() != BST_UNCHECKED);
}
