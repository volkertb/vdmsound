// AdvSettingsPage_Joy.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Joy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Joy property page

CAdvSettingsPage_Joy::CAdvSettingsPage_Joy(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Joy::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Joy)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Joy::~CAdvSettingsPage_Joy()
{
}

void CAdvSettingsPage_Joy::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Joy)
	DDX_Control(pDX, IDC_EDT_JOYMAPBROWSE, m_edtJoymapbrowse);
	DDX_Control(pDX, IDC_SPN_JOYSCALEMIN, m_spnJoyscalemin);
	DDX_Control(pDX, IDC_SPN_JOYSCALEMAX, m_spnJoyscalemax);
	DDX_Control(pDX, IDC_CMB_JOYSCALEMIN, m_cmbJoyscalemin);
	DDX_Control(pDX, IDC_CMB_JOYSCALEMAX, m_cmbJoyscalemax);
	DDX_Control(pDX, IDC_CMB_JOYPORT, m_cmbJoyport);
	DDX_Control(pDX, IDC_CMB_JOYPOLL, m_cmbJoypoll);
	DDX_Control(pDX, IDC_CHK_USEJOY, m_chkUsejoy);
	DDX_Control(pDX, IDC_BUT_JOYMAPBROWSE, m_butJoymapbrowse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Joy, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Joy)
	ON_BN_CLICKED(IDC_CHK_USEJOY, OnChkUsejoy)
	ON_BN_CLICKED(IDC_BUT_JOYMAPBROWSE, OnButJoymapbrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Joy::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.gameport"), _T("enabled"),   m_chkUsejoy,       TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsejoy.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Joy::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.gameport"), _T("port"),       m_cmbJoyport,      _T("0x201"));
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.gameport"), _T("minCoord"),   m_cmbJoyscalemin,  _T("5"));
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.gameport"), _T("maxCoord"),   m_cmbJoyscalemax,  _T("250"));
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.gameport"), _T("pollPeriod"), m_cmbJoypoll,      _T("125"));

  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.gameport"), _T("mapFile"),    m_edtJoymapbrowse, VLPUtil::GetVDMSFilePath(_T("joy2.map")));

  if (!bEnabled) {
    m_cmbJoyport.EnableWindow(FALSE);
    m_cmbJoyscalemin.EnableWindow(FALSE);
    m_cmbJoyscalemax.EnableWindow(FALSE);
    m_cmbJoypoll.EnableWindow(FALSE);

    m_butJoymapbrowse.EnableWindow(FALSE);
    m_spnJoyscalemin.EnableWindow(FALSE);
    m_spnJoyscalemax.EnableWindow(FALSE);
  } else {
    m_butJoymapbrowse.EnableWindow(TRUE);
    m_spnJoyscalemin.EnableWindow(TRUE);
    m_spnJoyscalemax.EnableWindow(TRUE);
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Joy message handlers

BOOL CAdvSettingsPage_Joy::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Setup controls
  m_spnJoyscalemin.SetRange(0, 500);
  m_spnJoyscalemax.SetRange(0, 500);

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Joy::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_JOYMAPBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Joy::OnApply() 
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

void CAdvSettingsPage_Joy::OnChkUsejoy() 
{
  SyncGUIData_Enabled(TRUE);  // save current combo-box selection
  SyncGUIData_Enabled(FALSE, m_chkUsejoy.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Joy::OnButJoymapbrowse() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  CString fileName;
  m_edtJoymapbrowse.GetWindowText(fileName);

  CFileDialog dlgFile(TRUE, NULL, fileName.IsEmpty() ? _T("") : VLPUtil::GetVDMSFilePath(fileName), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, VLPUtil::LoadString(IDS_TXT_FILTER3), this);

  switch (dlgFile.DoModal()) {
    case IDOK:
      m_edtJoymapbrowse.SetWindowText(dlgFile.GetPathName());
      break;

    case IDCANCEL:
      break;

    default:
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
  }
}
