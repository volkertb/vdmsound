// AdvSettingsPage_Sb.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Sb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb property page

CAdvSettingsPage_Sb::CAdvSettingsPage_Sb(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Sb::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Sb)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Sb::~CAdvSettingsPage_Sb()
{
}

void CAdvSettingsPage_Sb::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Sb)
	DDX_Control(pDX, IDC_EDT_SBOUTFILE, m_edtSboutfile);
	DDX_Control(pDX, IDC_CHK_SBOUTFILE, m_chkSboutfile);
	DDX_Control(pDX, IDC_BUT_SBOUTFILEBROWSE, m_butSboutfilebrowse);
	DDX_Control(pDX, IDC_SPN_SBOUTDEVBUF, m_spnSboutdevbuf);
	DDX_Control(pDX, IDC_CMB_SBTYPE, m_cmbSbtype);
	DDX_Control(pDX, IDC_CMB_SBPORT, m_cmbSbport);
	DDX_Control(pDX, IDC_CMB_SBOUTDEVBUF, m_cmbSboutdevbuf);
	DDX_Control(pDX, IDC_CMB_SBOUTDEV, m_cmbSboutdev);
	DDX_Control(pDX, IDC_CMB_SBIRQ, m_cmbSbirq);
	DDX_Control(pDX, IDC_CMB_SBDMA8, m_cmbSbdma8);
	DDX_Control(pDX, IDC_CMB_SBDMA16, m_cmbSbdma16);
	DDX_Control(pDX, IDC_CHK_USESB, m_chkUsesb);
	DDX_Control(pDX, IDC_CHK_SBOUTDEV, m_chkSboutdev);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Sb, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Sb)
	ON_BN_CLICKED(IDC_CHK_USESB, OnChkUsesb)
	ON_BN_CLICKED(IDC_CHK_SBOUTDEV, OnChkSboutdev)
	ON_BN_CLICKED(IDC_BUT_SBOUTFILEBROWSE, OnButSboutfilebrowse)
	ON_BN_CLICKED(IDC_CHK_SBOUTFILE, OnChkSboutfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Sb::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("enabled"),    m_chkUsesb,       TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsesb.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Sb::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("port"),       m_cmbSbport,      _T("0x220"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("IRQ"),        m_cmbSbirq,       _T("7"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("DMA8"),       m_cmbSbdma8,      _T("1"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("DMA16"),      m_cmbSbdma16,     _T("5"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("version"),    m_cmbSbtype,      _T("4.05 (SoundBlaster 16)"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("useDevOut"),  m_chkSboutdev,    TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("useFileOut"), m_chkSboutfile,   FALSE);
  SyncGUIData_Enabled_DevOut (bSave, bEnabled && (m_chkSboutdev.GetCheck()   != BST_UNCHECKED));
  SyncGUIData_Enabled_FileOut(bSave, bEnabled && (m_chkSboutfile.GetCheck()  != BST_UNCHECKED));

  if (!bEnabled) {
    m_cmbSbport.EnableWindow(FALSE);
    m_cmbSbirq.EnableWindow(FALSE);
    m_cmbSbdma8.EnableWindow(FALSE);
    m_cmbSbdma16.EnableWindow(FALSE);
    m_chkSboutdev.EnableWindow(FALSE);
    m_chkSboutfile.EnableWindow(FALSE);

    m_cmbSbtype.EnableWindow(FALSE);
  } else {
    m_cmbSbtype.EnableWindow(TRUE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Sb::SyncGUIData_Enabled_DevOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncDevListBox(bSave, m_settings, _T("vdms.sb.dsp"), m_devOutInfo, m_cmbSboutdev, _T("Out"), DeviceUtil::DEV_DSOUND, -1);
  VLPUtil::SyncEditBox   (bSave, m_settings, _T("vdms.sb.dsp"), _T("buffer"), m_cmbSboutdevbuf, _T("75"));

  if (!bEnabled) {
    m_cmbSboutdevbuf.EnableWindow(FALSE);
    m_cmbSboutdev.EnableWindow(FALSE);

    m_spnSboutdevbuf.EnableWindow(FALSE);
  } else {
    m_spnSboutdevbuf.EnableWindow(m_cmbSboutdevbuf.IsWindowEnabled());
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Sb::SyncGUIData_Enabled_FileOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("fileOut"), m_edtSboutfile, VLPUtil::GetVDMSFilePath(_T("")));

  if (!bEnabled) {
    m_edtSboutfile.EnableWindow(FALSE);
    m_butSboutfilebrowse.EnableWindow(FALSE);
  } else {
    m_butSboutfilebrowse.EnableWindow(TRUE);
  }

  return TRUE;
}

VOID CAdvSettingsPage_Sb::InitDeviceList(void) {
  m_devOutInfo.RemoveAll();
  m_cmbSboutdev.ResetContent();

  DeviceUtil::EnumWaveOut(m_devOutInfo);
  DeviceUtil::EnumDSoundOut(m_devOutInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb message handlers

BOOL CAdvSettingsPage_Sb::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
  // Setup controls
  m_spnSboutdevbuf.SetRange(0, 1500);
  InitDeviceList();

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

  // Register drag-n-drop handlers
  m_edtSboutfile.Register();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Sb::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_SBOUTFILEBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == EN_CHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Sb::OnApply() 
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

void CAdvSettingsPage_Sb::OnChkUsesb() 
{
  SyncGUIData_Enabled(TRUE);  // save current combo-box selection
  SyncGUIData_Enabled(FALSE, m_chkUsesb.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Sb::OnChkSboutdev() 
{
  SyncGUIData_Enabled_DevOut(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_DevOut(FALSE, m_chkSboutdev.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Sb::OnChkSboutfile() 
{
  SyncGUIData_Enabled_FileOut(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_FileOut(FALSE, m_chkSboutfile.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Sb::OnButSboutfilebrowse() 
{
  CWaitCursor wait;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CString dirName;

  if (VLPUtil::BrowseForFolder(dirName, BIF_RETURNONLYFSDIRS, VLPUtil::LoadString(IDS_TXT_BROWSEFOUTFOLDER_TIPS), NULL, this)) {
    m_settings.SetValue(_T("vdms.sb.dsp"), _T("fileOut"), dirName);
    SyncGUIData(FALSE);       // update the GUI to reflect any changed settings
  }
}
