// AdvSettingsPage_Adlib.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Adlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Adlib property page

CAdvSettingsPage_Adlib::CAdvSettingsPage_Adlib(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Adlib::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Adlib)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Adlib::~CAdvSettingsPage_Adlib()
{
}

void CAdvSettingsPage_Adlib::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Adlib)
	DDX_Control(pDX, IDC_EDT_FMOUTFILE, m_edtFmoutfile);
	DDX_Control(pDX, IDC_CHK_FMOUTFILE, m_chkFmoutfile);
	DDX_Control(pDX, IDC_SPN_FMOUTDEVBUF, m_spnFmoutdevbuf);
	DDX_Control(pDX, IDC_CMB_FMSYNTHRATE, m_cmbFmsynthrate);
	DDX_Control(pDX, IDC_CMB_FMPORT, m_cmbFmport);
	DDX_Control(pDX, IDC_CMB_FMOUTDEVBUF, m_cmbFmoutdevbuf);
	DDX_Control(pDX, IDC_CMB_FMOUTDEV, m_cmbFmoutdev);
	DDX_Control(pDX, IDC_CHK_USEFM, m_chkUsefm);
	DDX_Control(pDX, IDC_CHK_FMOUTDEV, m_chkFmoutdev);
	DDX_Control(pDX, IDC_BUT_FMOUTFILEBROWSE, m_butFmoutfilebrowse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Adlib, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Adlib)
	ON_BN_CLICKED(IDC_CHK_USEFM, OnChkUsefm)
	ON_BN_CLICKED(IDC_CHK_FMOUTDEV, OnChkFmoutdev)
	ON_BN_CLICKED(IDC_CHK_FMOUTFILE, OnChkFmoutfile)
	ON_BN_CLICKED(IDC_BUT_FMOUTFILEBROWSE, OnButFmoutfilebrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Adlib::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.fm"), _T("enabled"),    m_chkUsefm,       TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsefm.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Adlib::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.fm"), _T("port"),       m_cmbFmport,      _T("0x388"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.fm"), _T("sampleRate"), m_cmbFmsynthrate, _T("11025"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.fm"), _T("useDevOut"),  m_chkFmoutdev,    TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.fm"), _T("useFileOut"), m_chkFmoutfile,   FALSE);

  SyncGUIData_Enabled_DevOut (bSave, bEnabled && (m_chkFmoutdev.GetCheck()   != BST_UNCHECKED));
  SyncGUIData_Enabled_FileOut(bSave, bEnabled && (m_chkFmoutfile.GetCheck()  != BST_UNCHECKED));

  if (!bEnabled) {
    m_cmbFmport.EnableWindow(FALSE);
    m_cmbFmsynthrate.EnableWindow(FALSE);
    m_chkFmoutdev.EnableWindow(FALSE);
    m_chkFmoutfile.EnableWindow(FALSE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Adlib::SyncGUIData_Enabled_DevOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncDevListBox(bSave, m_settings, _T("vdms.sb.fm"), m_devOutInfo, m_cmbFmoutdev, _T("Out"), DeviceUtil::DEV_DSOUND, -1);
  VLPUtil::SyncEditBox   (bSave, m_settings, _T("vdms.sb.fm"), _T("buffer"), m_cmbFmoutdevbuf, _T("75"));

  if (!bEnabled) {
    m_cmbFmoutdevbuf.EnableWindow(FALSE);
    m_cmbFmoutdev.EnableWindow(FALSE);

    m_spnFmoutdevbuf.EnableWindow(FALSE);
  } else {
    m_spnFmoutdevbuf.EnableWindow(m_cmbFmoutdevbuf.IsWindowEnabled());
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Adlib::SyncGUIData_Enabled_FileOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.sb.fm"), _T("fileOut"), m_edtFmoutfile, VLPUtil::GetVDMSFilePath(_T("")));

  if (!bEnabled) {
    m_edtFmoutfile.EnableWindow(FALSE);
    m_butFmoutfilebrowse.EnableWindow(FALSE);
  } else {
    m_butFmoutfilebrowse.EnableWindow(TRUE);
  }

  return TRUE;
}

VOID CAdvSettingsPage_Adlib::InitDeviceList(void) {
  m_devOutInfo.RemoveAll();
  m_cmbFmoutdev.ResetContent();

  DeviceUtil::EnumWaveOut(m_devOutInfo);
  DeviceUtil::EnumDSoundOut(m_devOutInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Adlib message handlers

BOOL CAdvSettingsPage_Adlib::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Setup controls
  m_spnFmoutdevbuf.SetRange(0, 1500);
  InitDeviceList();

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Adlib::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_FMOUTFILEBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == EN_CHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Adlib::OnApply() 
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

void CAdvSettingsPage_Adlib::OnChkUsefm() 
{
  SyncGUIData_Enabled(TRUE);  // save current combo-box selection
  SyncGUIData_Enabled(FALSE, m_chkUsefm.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Adlib::OnChkFmoutdev() 
{
  SyncGUIData_Enabled_DevOut(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_DevOut(FALSE, m_chkFmoutdev.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Adlib::OnChkFmoutfile() 
{
  SyncGUIData_Enabled_FileOut(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_FileOut(FALSE, m_chkFmoutfile.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Adlib::OnButFmoutfilebrowse() 
{
  CWaitCursor wait;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CString dirName;

  if (VLPUtil::BrowseForFolder(dirName, BIF_RETURNONLYFSDIRS, VLPUtil::LoadString(IDS_TXT_BROWSEFOUTFOLDER_TIPS), NULL, this)) {
    m_settings.SetValue(_T("vdms.sb.fm"), _T("fileOut"), dirName);
    SyncGUIData(FALSE);       // update the GUI to reflect any changed settings
  }
}
