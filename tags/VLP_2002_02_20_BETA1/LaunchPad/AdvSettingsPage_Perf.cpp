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
 : CPropertyPage(CAdvSettingsPage_Perf::IDD), m_settings(settings), m_sliPriorityType(PRI_UNKNOWN)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Perf)
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Perf::~CAdvSettingsPage_Perf()
{
}

void CAdvSettingsPage_Perf::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Perf)
	DDX_Control(pDX, IDC_TXT_PRIORITY_MULTI, m_txtPrioritymulti);
	DDX_Control(pDX, IDC_SLI_PRIORITY, m_sliPriority);
	DDX_Control(pDX, IDC_CHK_DETECTIDLE, m_chkDetectidle);
	DDX_Control(pDX, IDC_CHK_COMPATTIMER, m_chkCompattimer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Perf, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Perf)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHK_DETECTIDLE, OnChkDetectidle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

BOOL CAdvSettingsPage_Perf::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.scheduling"), _T("compatHWEmu"), m_chkCompattimer, FALSE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.scheduling"), _T("detectIdle"),  m_chkDetectidle,  FALSE);
  SyncGUIData_DetectIdle(bSave, m_chkDetectidle.GetCheck() != BST_UNCHECKED);

  return TRUE;
}

BOOL CAdvSettingsPage_Perf::SyncGUIData_DetectIdle(BOOL bSave, BOOL bEnabled) {
  BOOL bEnabled2 = TRUE;

  if (bSave) {
    int curSel = m_sliPriority.GetPos();

    if ((m_sliPriorityType == PRI_DETERMINATE) || (curSel != 0)) {
      SetSlideState(TRUE);
      m_settings.SetValue   (_T("winnt.scheduling"), _T("idlePrio"), VLPUtil::FormatString(_T("%d"), curSel));
    } else {
      m_settings.UnsetValue (_T("winnt.scheduling"), _T("idlePrio"));
    }
  } else {
    CString value;
    BOOL isValueIndeterm;
    HRESULT hr;

    hr = m_settings.GetValue(_T("winnt.scheduling"), _T("idlePrio"), value, &isValueIndeterm, _T("100"));
    bEnabled2 &= SUCCEEDED(hr);

    LPTSTR valueEndPtr = NULL;
    LONG valueLong;
    valueLong = _tcstol(value, &valueEndPtr, 0);

    bEnabled2 &= ((valueEndPtr != NULL) && (valueEndPtr[0] == _T('\0')));

    if (isValueIndeterm) {
      SetSlideState(FALSE);
      m_sliPriority.SetPos(0);
    } else {
      SetSlideState(m_sliPriorityType != PRI_INDETERMINATE);
      m_sliPriority.SetPos(valueLong);
    }
  }

  if (!bEnabled) {
    m_sliPriority.EnableWindow(FALSE);
  } else {
    m_sliPriority.EnableWindow(TRUE);
  }

  return TRUE;
}

VOID CAdvSettingsPage_Perf::SetSlideState(BOOL isDeterminate) {
  if (isDeterminate) {
    m_sliPriorityType = PRI_DETERMINATE;
    m_sliPriority.SetTicFreq(10);
    m_txtPrioritymulti.ShowWindow(SW_HIDE);
  } else {
    m_sliPriorityType = PRI_INDETERMINATE;
    m_sliPriority.ClearTics(TRUE);
    m_txtPrioritymulti.ShowWindow(SW_SHOWNOACTIVATE);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Perf message handlers

BOOL CAdvSettingsPage_Perf::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

  // Setup controls
  m_sliPriority.SetRange(0, 100);
  m_sliPriority.SetLineSize(10);
  m_sliPriority.SetPageSize(50);
  m_sliPriority.SetTipSide(TBTS_TOP);

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

  if ((GetDlgItem(nID) != NULL) &&
      (nCode == BN_CLICKED))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

  return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Perf::OnApply() 
{
  CWaitCursor wait;

  SyncGUIData(TRUE);            // save all changes that occured in the GUI

  CWnd* pWndParent = GetParent();

  ASSERT_VALID(pWndParent);

  if (pWndParent != NULL) {
    pWndParent->SendMessage(WM_SAVESETTINGS);
  }

  return CPropertyPage::OnApply();
}

void CAdvSettingsPage_Perf::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if (nSBCode == TB_ENDTRACK) {
    SetSlideState(TRUE);        // if the user scrolled to a value then get into a determinate state
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAdvSettingsPage_Perf::OnChkDetectidle() 
{
  SyncGUIData_DetectIdle(TRUE); // save current combo-box selection
  SyncGUIData_DetectIdle(FALSE, m_chkDetectidle.GetCheck() != BST_UNCHECKED);
}
