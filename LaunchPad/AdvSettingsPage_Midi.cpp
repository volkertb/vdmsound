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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

VOID CAdvSettingsPage_Midi::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("enabled"),      m_chkUsempu,    TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsempu.GetCheck() != BST_UNCHECKED);
}

VOID CAdvSettingsPage_Midi::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.midi"), _T("port"),         m_cmbMpuport,   _T("0x330"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.midi"), _T("IRQ"),          m_cmbMpuirq,    _T("2"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.midi"), _T("useDevice"),    m_chkMpuoutdev, TRUE);
  SyncGUIData_Enabled_Device(bSave, bEnabled && (m_chkMpuoutdev.GetCheck() != BST_UNCHECKED));

  // TODO: read/write sysex

  if (!bEnabled) {
    m_cmbMpuport.EnableWindow(FALSE);
    m_cmbMpuirq.EnableWindow(FALSE);
    m_chkMpuoutdev.EnableWindow(FALSE);

    m_cmbSysexindicator.EnableWindow(FALSE);
    m_butMidimapbrowse.EnableWindow(FALSE);
  } else {
    m_cmbSysexindicator.EnableWindow(TRUE);
    m_butMidimapbrowse.EnableWindow(TRUE);
  }
}

VOID CAdvSettingsPage_Midi::SyncGUIData_Enabled_Device(BOOL bSave, BOOL bEnabled) {
  // TODO: read/write device

  if (!bEnabled) {
    m_cmbMpuoutdev.EnableWindow(FALSE);
  } else {
    m_cmbMpuoutdev.EnableWindow(TRUE);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi message handlers

BOOL CAdvSettingsPage_Midi::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
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
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
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

void CAdvSettingsPage_Midi::OnChkMpuoutdev() 
{
  SyncGUIData_Enabled_Device(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_Device(FALSE, m_chkMpuoutdev.GetCheck() != BST_UNCHECKED);
}
