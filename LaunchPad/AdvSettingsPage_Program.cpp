// AdvSettingsPage_Program.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"
#include "AdvSettingsPage_Program.h"

#include "ChangeIconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Program property page

CAdvSettingsPage_Program::CAdvSettingsPage_Program(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Program::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Program)
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Program::~CAdvSettingsPage_Program()
{
}

void CAdvSettingsPage_Program::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Program)
	DDX_Control(pDX, IDC_EDT_DOSPROGRAM, m_edtDosprogram);
	DDX_Control(pDX, IDC_EDT_DOSDIR, m_edtDosdir);
	DDX_Control(pDX, IDC_EDT_DOSARGS, m_edtDosargs);
	DDX_Control(pDX, IDC_CHK_WINKPRTSC, m_chkWinkprtsc);
	DDX_Control(pDX, IDC_CHK_WINKCTRLESC, m_chkWinkctrlesc);
	DDX_Control(pDX, IDC_CHK_WINKALTTAB, m_chkWinkalttab);
	DDX_Control(pDX, IDC_CHK_WINKALTSPACE, m_chkWinkaltspace);
	DDX_Control(pDX, IDC_CHK_WINKALTPRTSC, m_chkWinkaltprtsc);
	DDX_Control(pDX, IDC_CHK_WINKALTESC, m_chkWinkaltesc);
	DDX_Control(pDX, IDC_CHK_WINKALTENTER, m_chkWinkaltenter);
	DDX_Control(pDX, IDC_CHK_WARN, m_chkWarn);
	DDX_Control(pDX, IDC_CHK_FASTPASTE, m_chkFastpaste);
	DDX_Control(pDX, IDC_CHK_CLOSE, m_chkClose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Program, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Program)
	ON_BN_CLICKED(IDC_BUT_DOSPROGRAMBROWSE, OnButDosprogrambrowse)
	ON_BN_CLICKED(IDC_BUT_DOSDIRBROWSE, OnButDosdirbrowse)
	ON_BN_CLICKED(IDC_BUT_CHANGEICON, OnButChangeicon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage helper functions

VOID CAdvSettingsPage_Program::SyncGUIData(BOOL bSave) {
  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  // Program information
  VLPUtil::SyncEditBox (bSave, m_settings, _T("program"), _T("executable"), m_edtDosprogram);
  VLPUtil::SyncEditBox (bSave, m_settings, _T("program"), _T("params"), m_edtDosargs);
  VLPUtil::SyncEditBox (bSave, m_settings, _T("program"), _T("workdir"), m_edtDosdir);

  // Misc
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dosbox"), _T("exitClose"), m_chkClose,        FALSE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dosbox"), _T("exitWarn"),  m_chkWarn,         TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.dosbox"), _T("fastPaste"), m_chkFastpaste,    TRUE);

  // Windows shortcut keys
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("altTab"),    m_chkWinkalttab,   TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("altEsc"),    m_chkWinkaltesc,   TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("ctlEsc"),    m_chkWinkctrlesc,  TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("prscr"),     m_chkWinkprtsc,    TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("altPrscr"),  m_chkWinkaltprtsc, TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("altEnter"),  m_chkWinkaltenter, TRUE);
  VLPUtil::SyncCheckBox(bSave, m_settings, _T("winnt.keys"),   _T("altSpace"),  m_chkWinkaltspace, TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Program message handlers

BOOL CAdvSettingsPage_Program::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
  // Load the information from file into the GUI
  SyncGUIData(FALSE);

  // This page was not modified yet
//  SetModified(FALSE);           // enable the "Apply" button to reflect the fact that changes were made

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAdvSettingsPage_Program::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

  ASSERT(::GetDlgItem(m_hWnd, nID) == hWndCtrl);

  if ((GetDlgItem(nID) != NULL) &&
      (nID != IDC_BUT_DOSPROGRAMBROWSE) && (nID != IDC_BUT_DOSDIRBROWSE) && (nID != IDC_BUT_CHANGEICON) &&
      ((nCode == EN_CHANGE) || (nCode == BN_CLICKED)))
  {
    SetModified();              // enable the "Apply" button to reflect the fact that changes were made
  }

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAdvSettingsPage_Program::OnApply() 
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

void CAdvSettingsPage_Program::OnButDosprogrambrowse() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CString edtDosprogram, edtDosdir;
  m_edtDosprogram.GetWindowText(edtDosprogram);
  m_edtDosdir.GetWindowText(edtDosdir);

	COpenDOSProgramDialog dlgFile(VLPUtil::GetAbsolutePath(edtDosprogram, edtDosdir), this);

  switch (dlgFile.DoModal()) {
    case IDOK:
      m_settings.SetValue(_T("program"), _T("executable"), dlgFile.GetPathName());
      SyncGUIData(FALSE);       // update the GUI to reflect any changed settings

      if (m_settings.IsChanged())
        SetModified();      // enable the "Apply" button to reflect the fact that changes were made

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

void CAdvSettingsPage_Program::OnButDosdirbrowse() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  CString txtTitle;
  txtTitle.LoadString(IDS_TXT_BROWSEFOLDER_TIPS);

  TCHAR tmpBuf[MAX_PATH];

  BROWSEINFO browseInfo;
  browseInfo.hwndOwner = m_hWnd;
  browseInfo.pidlRoot  = NULL;
  browseInfo.pszDisplayName = tmpBuf;
  browseInfo.lpszTitle = txtTitle;
  browseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
  browseInfo.lpfn      = NULL;
  browseInfo.lParam    = 0;
  browseInfo.iImage    = 0;

  LPITEMIDLIST pidlBrowse = SHBrowseForFolder(&browseInfo);

  if (pidlBrowse != NULL) {
    if (SHGetPathFromIDList(pidlBrowse, tmpBuf)) {
      m_settings.SetValue(_T("program"), _T("workdir"), tmpBuf);
      SyncGUIData(FALSE);       // update the GUI to reflect any changed settings

      if (m_settings.IsChanged())
        SetModified();      // enable the "Apply" button to reflect the fact that changes were made
    } else {
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
    }

    HRESULT hr;
    CComPtr<IMalloc> pMalloc;

    if (FAILED(hr = SHGetMalloc(&pMalloc))) {
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, hr, (LPCTSTR)VLPUtil::FormatMessage(hr, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
    } else {
      pMalloc->Free(pidlBrowse);
    }
  }
}

void CAdvSettingsPage_Program::OnButChangeicon() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;
  CChangeIconDlg dlgIcon(this);

  m_settings.GetValue(_T("program"), _T("icon"), dlgIcon.m_edtIcofile_val);

  switch (dlgIcon.DoModal()) {
    case IDOK:
      strTmp1.Format(_T("%s,%d"), dlgIcon.m_edtIcofile_val, max(0, dlgIcon.m_lstIcons_val));
      m_settings.SetValue(_T("program"), _T("icon"), strTmp1);

      if (m_settings.IsChanged())
        SetModified();      // enable the "Apply" button to reflect the fact that changes were made

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
