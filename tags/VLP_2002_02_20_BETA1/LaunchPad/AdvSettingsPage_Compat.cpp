// AdvSettingsPage_Compat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Compat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Compat property page

CAdvSettingsPage_Compat::CAdvSettingsPage_Compat(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Compat::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Compat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Compat::~CAdvSettingsPage_Compat()
{
}

void CAdvSettingsPage_Compat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Compat)
  DDX_Control(pDX, IDC_CMB_XMS, m_cmbXms);
	DDX_Control(pDX, IDC_CMB_EMS, m_cmbEms);
	DDX_Control(pDX, IDC_CHK_XMS, m_chkXms);
	DDX_Control(pDX, IDC_CHK_VESA, m_chkVesa);
	DDX_Control(pDX, IDC_CHK_NETWARE, m_chkNetware);
	DDX_Control(pDX, IDC_CHK_EMS, m_chkEms);
	DDX_Control(pDX, IDC_CHK_DPMI, m_chkDpmi);
	DDX_Control(pDX, IDC_CHK_CDROM, m_chkCdrom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Compat, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Compat)
	ON_BN_CLICKED(IDC_CHK_EMS, OnChkEms)
	ON_BN_CLICKED(IDC_CHK_XMS, OnChkXms)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Compat::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  // Memory
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.memory"),  _T("useEMS"),     m_chkEms,     TRUE);
  SyncGUIData_EMS(bSave, m_chkEms.GetCheck() != BST_UNCHECKED);

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.memory"),  _T("useXMS"),     m_chkXms,     TRUE);
  SyncGUIData_XMS(bSave, m_chkXms.GetCheck() != BST_UNCHECKED);

  // Protected mode
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.pmode"),   _T("useDPMI"),    m_chkDpmi,    TRUE);

  // Video
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.video"),   _T("useVESA"),    m_chkVesa,    FALSE);

  // Others
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.storage"), _T("useCDROM"),   m_chkCdrom,   FALSE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.storage"), _T("useNetware"), m_chkNetware, FALSE);

  return TRUE;
}

BOOL CAdvSettingsPage_Compat::SyncGUIData_EMS(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("winnt.memory"), _T("memEMS"), m_cmbEms, _T("4096"));

  if (!bEnabled) {
    m_cmbEms.EnableWindow(FALSE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Compat::SyncGUIData_XMS(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("winnt.memory"), _T("memXMS"), m_cmbXms, _T("16384"));

  if (!bEnabled) {
    m_cmbXms.EnableWindow(FALSE);
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Compat message handlers

BOOL CAdvSettingsPage_Compat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Compat::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Compat::OnApply() 
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

void CAdvSettingsPage_Compat::OnChkEms() 
{
  SyncGUIData_EMS(TRUE);     // save current combo-box selection
  SyncGUIData_EMS(FALSE, m_chkEms.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Compat::OnChkXms() 
{
  SyncGUIData_XMS(TRUE);     // save current combo-box selection
  SyncGUIData_XMS(FALSE, m_chkXms.GetCheck() != BST_UNCHECKED);
}
