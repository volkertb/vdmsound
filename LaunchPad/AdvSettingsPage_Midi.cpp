// AdvSettingsPage_Midi.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Midi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi property page

CAdvSettingsPage_Midi::CAdvSettingsPage_Midi(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Midi::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Midi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Midi::~CAdvSettingsPage_Midi()
{
}

void CAdvSettingsPage_Midi::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Midi)
	DDX_Control(pDX, IDC_BUT_MPUOUTFILEBROWSE, m_butMpuoutfilebrowse);
	DDX_Control(pDX, IDC_EDT_MPUOUTFILE, m_edtMpuoutfile);
	DDX_Control(pDX, IDC_CMB_MPUINDEV, m_cmbMpuindev);
	DDX_Control(pDX, IDC_CHK_MPUOUTFILE, m_chkMpuoutfile);
	DDX_Control(pDX, IDC_CHK_MPUINDEV, m_chkMpuindev);
	DDX_Control(pDX, IDC_CHK_SYSEXINDICATOR, m_chkSysexindicator);
	DDX_Control(pDX, IDC_EDT_MIDIMAPBROWSE, m_edtMidimapbrowse);
	DDX_Control(pDX, IDC_CMB_SYSEXINDICATOR, m_cmbSysexindicator);
	DDX_Control(pDX, IDC_CMB_MPUPORT, m_cmbMpuport);
	DDX_Control(pDX, IDC_CMB_MPUOUTDEV, m_cmbMpuoutdev);
	DDX_Control(pDX, IDC_CMB_MPUIRQ, m_cmbMpuirq);
	DDX_Control(pDX, IDC_CHK_USEMPU, m_chkUsempu);
	DDX_Control(pDX, IDC_CHK_MPUOUTDEV, m_chkMpuoutdev);
	DDX_Control(pDX, IDC_BUT_MIDIMAPBROWSE, m_butMidimapbrowse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Midi, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Midi)
	ON_BN_CLICKED(IDC_CHK_USEMPU, OnChkUsempu)
	ON_BN_CLICKED(IDC_CHK_MPUOUTDEV, OnChkMpuoutdev)
	ON_BN_CLICKED(IDC_BUT_MIDIMAPBROWSE, OnButMidimapbrowse)
	ON_BN_CLICKED(IDC_CHK_SYSEXINDICATOR, OnChkSysexindicator)
	ON_BN_CLICKED(IDC_CHK_MPUINDEV, OnChkMpuindev)
	ON_BN_CLICKED(IDC_CHK_MPUOUTFILE, OnChkMpuoutfile)
	ON_BN_CLICKED(IDC_BUT_MPUOUTFILEBROWSE, OnButMpuoutfilebrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Midi::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("enabled"),      m_chkUsempu,         TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsempu.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Midi::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.midi"), _T("port"),         m_cmbMpuport,        _T("0x330"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.midi"), _T("IRQ"),          m_cmbMpuirq,         _T("2"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("useSysExLed"),  m_chkSysexindicator, TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("useDevOut"),    m_chkMpuoutdev,      TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("useFileOut"),   m_chkMpuoutfile,     FALSE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("useDevIn"),     m_chkMpuindev,       FALSE);

  SyncGUIData_Enabled_SysEx  (bSave, bEnabled && (m_chkSysexindicator.GetCheck() != BST_UNCHECKED));
  SyncGUIData_Enabled_DevOut (bSave, bEnabled && (m_chkMpuoutdev.GetCheck()      != BST_UNCHECKED));
  SyncGUIData_Enabled_FileOut(bSave, bEnabled && (m_chkMpuoutfile.GetCheck()     != BST_UNCHECKED));
  SyncGUIData_Enabled_DevIn  (bSave, bEnabled && (m_chkMpuindev.GetCheck()       != BST_UNCHECKED));

  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.midi"), _T("mapFile"),      m_edtMidimapbrowse, VLPUtil::GetVDMSFilePath(_T("identity.map")));

  if (!bEnabled) {
    m_cmbMpuport.EnableWindow(FALSE);
    m_cmbMpuirq.EnableWindow(FALSE);
    m_chkSysexindicator.EnableWindow(FALSE);
    m_chkMpuoutdev.EnableWindow(FALSE);
    m_chkMpuoutfile.EnableWindow(FALSE);
    m_chkMpuindev.EnableWindow(FALSE);

    m_butMidimapbrowse.EnableWindow(FALSE);
  } else {
    m_butMidimapbrowse.EnableWindow(TRUE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Midi::SyncGUIData_Enabled_SysEx(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncComboBox(bSave, m_settings, _T("vdms.midi"), _T("sysExLed"), m_cmbSysexindicator, _T("scroll"));

  if (!bEnabled) {
    m_cmbSysexindicator.EnableWindow(FALSE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Midi::SyncGUIData_Enabled_DevOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncDevListBox(bSave, m_settings, _T("vdms.midi"), m_devOutInfo, m_cmbMpuoutdev, _T("Out"), DeviceUtil::DEV_MIDI, -1);

  if (!bEnabled) {
    m_cmbMpuoutdev.EnableWindow(FALSE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Midi::SyncGUIData_Enabled_FileOut(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox(bSave, m_settings, _T("vdms.midi"), _T("fileOut"), m_edtMpuoutfile, VLPUtil::GetVDMSFilePath(_T("")));

  if (!bEnabled) {
    m_edtMpuoutfile.EnableWindow(FALSE);
    m_butMpuoutfilebrowse.EnableWindow(FALSE);
  } else {
    m_butMpuoutfilebrowse.EnableWindow(TRUE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Midi::SyncGUIData_Enabled_DevIn(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncDevListBox(bSave, m_settings, _T("vdms.midi"), m_devInInfo, m_cmbMpuindev, _T("In"), DeviceUtil::DEV_MIDI, 0);

  if (!bEnabled) {
    m_cmbMpuindev.EnableWindow(FALSE);
  }

  return TRUE;
}

VOID CAdvSettingsPage_Midi::InitDeviceList(void) {
  m_devOutInfo.RemoveAll();
  m_devInInfo.RemoveAll();
  m_cmbMpuoutdev.ResetContent();
  m_cmbMpuindev.ResetContent();

  DeviceUtil::EnumMidiOut(m_devOutInfo);
  DeviceUtil::EnumMidiIn(m_devInInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi message handlers

BOOL CAdvSettingsPage_Midi::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Set up controls
  InitDeviceList();

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Midi::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_MIDIMAPBROWSE) && (nID != IDC_BUT_MPUOUTFILEBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == EN_CHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Midi::OnApply() 
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

void CAdvSettingsPage_Midi::OnChkUsempu() 
{
  SyncGUIData_Enabled(TRUE);  // save current combo-box selection
  SyncGUIData_Enabled(FALSE, m_chkUsempu.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Midi::OnChkSysexindicator() 
{
  SyncGUIData_Enabled_SysEx(TRUE);  // save current combo-box selection
  SyncGUIData_Enabled_SysEx(FALSE, m_chkSysexindicator.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Midi::OnChkMpuoutdev() 
{
  SyncGUIData_Enabled_DevOut(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_DevOut(FALSE, m_chkMpuoutdev.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Midi::OnButMidimapbrowse() 
{
  CWaitCursor wait;

  CString fileName;
  m_edtMidimapbrowse.GetWindowText(fileName);

  if (VLPUtil::BrowseForFile(fileName, TRUE, NULL, fileName.IsEmpty() ? _T("") : VLPUtil::GetVDMSFilePath(fileName), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, VLPUtil::LoadString(IDS_TXT_FILTER3), this)) {
    m_edtMidimapbrowse.SetWindowText(fileName);
  }
}

void CAdvSettingsPage_Midi::OnChkMpuindev() 
{
  SyncGUIData_Enabled_DevIn(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_DevIn(FALSE, m_chkMpuindev.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Midi::OnChkMpuoutfile() 
{
  SyncGUIData_Enabled_FileOut(TRUE);   // save current selection
  SyncGUIData_Enabled_FileOut(FALSE, m_chkMpuoutfile.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Midi::OnButMpuoutfilebrowse() 
{
  CWaitCursor wait;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CString dirName;

  if (VLPUtil::BrowseForFolder(dirName, BIF_RETURNONLYFSDIRS, VLPUtil::LoadString(IDS_TXT_BROWSEFOUTFOLDER_TIPS), NULL, this)) {
    m_settings.SetValue(_T("vdms.midi"), _T("fileOut"), dirName);
    SyncGUIData(FALSE);       // update the GUI to reflect any changed settings
  }
}
