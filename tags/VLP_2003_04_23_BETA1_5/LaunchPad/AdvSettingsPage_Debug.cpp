// AdvSettingsPage_Debug.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Debug property page

CAdvSettingsPage_Debug::CAdvSettingsPage_Debug(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Debug::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Debug)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Debug::~CAdvSettingsPage_Debug()
{
}

void CAdvSettingsPage_Debug::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Debug)
	DDX_Control(pDX, IDC_EDT_VDMSINI, m_edtVdmsini);
	DDX_Control(pDX, IDC_CMB_LOGDETAIL, m_cmbLogdetail);
	DDX_Control(pDX, IDC_CHK_VDMSLOG, m_chkVdmslog);
	DDX_Control(pDX, IDC_CHK_VDMSINI, m_chkVdmsini);
	DDX_Control(pDX, IDC_CHK_LOGSB, m_chkLogsb);
	DDX_Control(pDX, IDC_CHK_LOGOTHER, m_chkLogother);
	DDX_Control(pDX, IDC_CHK_LOGMIDI, m_chkLogmidi);
	DDX_Control(pDX, IDC_CHK_LOGJOY, m_chkLogjoy);
	DDX_Control(pDX, IDC_CHK_LOGADLIB, m_chkLogadlib);
	DDX_Control(pDX, IDC_BUT_LOGBROWSE, m_butLogbrowse);
	DDX_Control(pDX, IDC_EDT_LOGBROWSE, m_edtLogbrowse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Debug, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Debug)
	ON_BN_CLICKED(IDC_CHK_VDMSLOG, OnChkVdmslog)
	ON_BN_CLICKED(IDC_CHK_VDMSINI, OnChkVdmsini)
	ON_BN_CLICKED(IDC_BUT_LOGBROWSE, OnButLogbrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Debug::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logEnabled"),   m_chkVdmslog,  FALSE);
  SyncGUIData_VDMSLOG(bSave, m_chkVdmslog.GetCheck() != BST_UNCHECKED);

  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("useCustomCfg"), m_chkVdmsini,  FALSE);
  SyncGUIData_VDMSINI(bSave, m_chkVdmsini.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Debug::SyncGUIData_VDMSLOG(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.debug"), _T("logFile"),      m_edtLogbrowse, _T(".\\VDMS.LOG"));
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logMidi"),      m_chkLogmidi,   TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logAdLib"),     m_chkLogadlib,  TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logSBlaster"),  m_chkLogsb,     TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logJoystick"),  m_chkLogjoy,    TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("vdms.debug"), _T("logOther"),     m_chkLogother,  TRUE);

  const LONG log_levels[] = { 192, 128, 0 };

  BOOL bEnabled2 = TRUE;

  if (bSave) {
    int curSel = m_cmbLogdetail.GetCurSel();

    if ((curSel != CB_ERR) && (curSel >= 0) && (curSel < sizeof(log_levels)/sizeof(log_levels[0]))) {
      m_settings.SetValue   (_T("vdms.debug"), _T("logDetail"), VLPUtil::FormatString(_T("%d"), log_levels[curSel]));
    } else {
      m_settings.UnsetValue (_T("vdms.debug"), _T("logDetail"));
    }
  } else {
    CString value;
    BOOL isValueIndeterm;
    HRESULT hr;

    hr = m_settings.GetValue(_T("vdms.debug"), _T("logDetail"), value, &isValueIndeterm, _T("0"));
    bEnabled2 &= SUCCEEDED(hr);

    LPTSTR valueEndPtr = NULL;
    LONG valueLong;
    valueLong = _tcstol(value, &valueEndPtr, 0);
    bEnabled2 &= ((valueEndPtr != NULL) && (valueEndPtr[0] == _T('\0')));

    if (isValueIndeterm) {
      m_cmbLogdetail.SetCurSel(-1);
    } else {
      for (int i = 0; i < sizeof(log_levels)/sizeof(log_levels[0]); i++) {
        if (valueLong >= log_levels[i]) {
          m_cmbLogdetail.SetCurSel(i);
          break;
        }
      }
    }
  }

  if (!bEnabled) {
    m_chkLogmidi.EnableWindow(FALSE);
    m_chkLogmidi.EnableWindow(FALSE);
    m_chkLogadlib.EnableWindow(FALSE);
    m_chkLogsb.EnableWindow(FALSE);
    m_chkLogjoy.EnableWindow(FALSE);
    m_chkLogother.EnableWindow(FALSE);

    m_cmbLogdetail.EnableWindow(FALSE);
    m_butLogbrowse.EnableWindow(FALSE);
  } else {
    m_cmbLogdetail.EnableWindow(bEnabled2);
    m_butLogbrowse.EnableWindow(TRUE);
  }

  return TRUE;
}

BOOL CAdvSettingsPage_Debug::SyncGUIData_VDMSINI(BOOL bSave, BOOL bEnabled) {
  VLPUtil::SyncEditBox (bSave, m_settings, _T("vdms.debug"),  _T("customCfg"),   m_edtVdmsini,  _T(""));

  if (!bEnabled) {
    m_edtVdmsini.EnableWindow(FALSE);
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Debug message handlers

BOOL CAdvSettingsPage_Debug::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Load the information from file into the GUI
  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Debug::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_LOGBROWSE) && (nID != IDC_BUT_MPUOUTFILEBROWSE) &&
      ((nCode == CBN_SELCHANGE) || (nCode == CBN_EDITCHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Debug::OnApply() 
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

void CAdvSettingsPage_Debug::OnChkVdmslog() 
{
  SyncGUIData_VDMSLOG(TRUE);  // save current selection
  SyncGUIData_VDMSLOG(FALSE, m_chkVdmslog.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Debug::OnChkVdmsini() 
{
  SyncGUIData_VDMSINI(TRUE);  // save current selection
  SyncGUIData_VDMSINI(FALSE, m_chkVdmsini.GetCheck() != BST_UNCHECKED);
}

void CAdvSettingsPage_Debug::OnButLogbrowse() 
{
  CWaitCursor wait;

  CString fileName;
  m_edtLogbrowse.GetWindowText(fileName);

  if (VLPUtil::BrowseForFile(fileName, FALSE, VLPUtil::LoadString(IDS_TXT_FILTER4_DEFEXT), fileName.IsEmpty() ? _T("") : VLPUtil::GetVDMSFilePath(fileName), OFN_HIDEREADONLY, VLPUtil::LoadString(IDS_TXT_FILTER4), this)) {
    m_edtLogbrowse.SetWindowText(fileName);
  }
}
