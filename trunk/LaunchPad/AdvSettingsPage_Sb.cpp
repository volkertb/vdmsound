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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

VOID CAdvSettingsPage_Sb::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("enabled"),    m_chkUsesb,       TRUE);
  SyncGUIData_Enabled(bSave, m_chkUsesb.GetCheck() != BST_UNCHECKED);
}

VOID CAdvSettingsPage_Sb::SyncGUIData_Enabled(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("port"),       m_cmbSbport,      _T("0x220"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("IRQ"),        m_cmbSbirq,       _T("7"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("DMA8"),       m_cmbSbdma8,      _T("1"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("DMA16"),      m_cmbSbdma16,     _T("5"));
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("version"),    m_cmbSbtype,      _T("4.05 (SoundBlaster 16)"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.sb.dsp"), _T("useDevice"),  m_chkSboutdev,    TRUE);
  SyncGUIData_Enabled_Device(bSave, bEnabled && (m_chkSboutdev.GetCheck() != BST_UNCHECKED));

  if (!bEnabled) {
    m_cmbSbport.EnableWindow(FALSE);
    m_cmbSbirq.EnableWindow(FALSE);
    m_cmbSbdma8.EnableWindow(FALSE);
    m_cmbSbdma16.EnableWindow(FALSE);
    m_chkSboutdev.EnableWindow(FALSE);

    m_cmbSbtype.EnableWindow(FALSE);
  } else {
    m_cmbSbtype.EnableWindow(TRUE);
  }
}

VOID CAdvSettingsPage_Sb::SyncGUIData_Enabled_Device(BOOL bSave, BOOL bEnabled) {
  // Read/write device
  if (bSave) {
    int curSel = m_cmbSboutdev.GetCurSel();

    if (curSel != CB_ERR) {
      m_settings.SetValue(_T("vdms.sb.dsp"), _T("deviceType"), VLPUtil::FormatString(_T("%d"), m_devInfo[curSel].deviceType));
      m_settings.SetValue(_T("vdms.sb.dsp"), _T("deviceID"),   VLPUtil::FormatString(_T("%d"), m_devInfo[curSel].deviceID));
    }
  } else {
    BOOL isDevTypeIndeterm, isDevIDIndeterm;
    CString devType, devID;

    if (FAILED(m_settings.GetValue(_T("vdms.sb.dsp"), _T("deviceType"), devType, &isDevTypeIndeterm, _T("2")))  ||
        FAILED(m_settings.GetValue(_T("vdms.sb.dsp"), _T("deviceID"),   devID,   &isDevIDIndeterm,   _T("-1"))) ||
        isDevTypeIndeterm ||
        isDevIDIndeterm)
    {
      m_cmbSboutdev.SetCurSel(-1);
    } else {
      for (int i = 0; i < m_devInfo.GetSize(); i++) {
        if ((m_devInfo[i].deviceType == (DeviceUtil::DeviceType)_tcstol(devType, NULL, 10)) && (m_devInfo[i].deviceID == (UINT)_tcstol(devID, NULL, 10))) {
          m_cmbSboutdev.SetCurSel(i);
          break;
        }
      }

      if (i >= m_devInfo.GetSize()) {
        m_cmbSboutdev.SetCurSel(-1);
      }
    }
  }

  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.sb.dsp"), _T("buffer"),     m_cmbSboutdevbuf, _T("75"));

  if (!bEnabled) {
    m_cmbSboutdevbuf.EnableWindow(FALSE);

    m_cmbSboutdev.EnableWindow(FALSE);
    m_spnSboutdevbuf.EnableWindow(FALSE);
  } else {
    m_cmbSboutdev.EnableWindow(TRUE);
    m_spnSboutdevbuf.EnableWindow(TRUE);
  }
}

VOID CAdvSettingsPage_Sb::InitDeviceList(void) {
  m_devInfo.RemoveAll();
  m_cmbSboutdev.ResetContent();

  DeviceUtil::EnumWaveOut(m_devInfo);
  DeviceUtil::EnumDSoundOut(m_devInfo);

  for (int i = 0; i < m_devInfo.GetSize(); i++) {
    m_cmbSboutdev.AddString(m_devInfo[i].deviceName + _T(" (") + DeviceUtil::GetDevTypeText(m_devInfo[i].deviceType) + _T(")"));
  }

  ASSERT(m_cmbSboutdev.GetCount() == m_devInfo.GetSize());
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb message handlers

BOOL CAdvSettingsPage_Sb::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
  // Setup controls
  m_spnSboutdevbuf.SetRange(0, 500);
  InitDeviceList();

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

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
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
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
  SyncGUIData_Enabled_Device(TRUE); // save current combo-box selection
  SyncGUIData_Enabled_Device(FALSE, m_chkSboutdev.GetCheck() != BST_UNCHECKED);
}
