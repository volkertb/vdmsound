// BasicSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BasicSettingsPage.h"

#include "BasicBrowseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage property page

IMPLEMENT_DYNCREATE(CBasicSettingsPage, CPropertyPage)

CBasicSettingsPage::CBasicSettingsPage(const CStringArray& fileNames) : CPropertyPage(CBasicSettingsPage::IDD), m_settings(fileNames)
{
	//{{AFX_DATA_INIT(CBasicSettingsPage)
	//}}AFX_DATA_INIT
}

CBasicSettingsPage::~CBasicSettingsPage()
{
}

//
// Adds the property page to a property sheet using the supplied
//  callback function and long value
//
HRESULT CBasicSettingsPage::AddPage(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam, const CStringArray& fileNames) {
  // Create a self-destroying instance of CBasicSettingsPage
  CBasicSettingsPage* pBasicSettingsPage = new CBasicSettingsPage(fileNames);

  ASSERT_VALID(pBasicSettingsPage);

  if (!pBasicSettingsPage)
    return E_OUTOFMEMORY;

  PROPSHEETPAGE psp;
  memset(&psp, 0, sizeof(psp));

  psp.dwSize      = sizeof(psp);
  psp.dwFlags     = pBasicSettingsPage->m_psp.dwFlags | PSP_USEICONID | PSP_USECALLBACK | PSP_USEREFPARENT;
  psp.hInstance   = pBasicSettingsPage->m_psp.hInstance;
  psp.pszTemplate = pBasicSettingsPage->m_psp.pszTemplate;
  psp.pszIcon     = (LPCTSTR)MAKEINTRESOURCE(IDI_APPICON);
  psp.pszTitle    = pBasicSettingsPage->m_psp.pszTitle;
  psp.pfnDlgProc  = pBasicSettingsPage->m_psp.pfnDlgProc;
  psp.lParam      = (LONG)pBasicSettingsPage;
  psp.pfnCallback = PropPageCallbackProc;
  psp.pcRefParent = (UINT*)(&(_Module.m_nLockCnt));   // prevent the DLL from being unloaded while the property sheet is active

  // Obtain the Win32 property sheet handle
  HPROPSHEETPAGE hPage = CreatePropertySheetPage(&psp);

  if (!hPage) {
    delete pBasicSettingsPage;
    return HRESULT_FROM_WIN32(GetLastError());
  }

  // Call the callback function to add the page to a property sheet
  if (!lpfnAddPageProc(hPage, lParam)) {
    DestroyPropertySheetPage(hPage);
    delete pBasicSettingsPage;
    return E_UNEXPECTED;
  }

  return S_OK;
}

UINT CALLBACK CBasicSettingsPage::PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp) {
  UINT retVal = TRUE;

  ASSERT(ppsp != NULL);
  CBasicSettingsPage* pBasicSettingsPage = (CBasicSettingsPage*)(ppsp->lParam);
  ASSERT_VALID(pBasicSettingsPage);

  if (pBasicSettingsPage->m_psp.dwFlags & PSP_USECALLBACK) {
    retVal = (pBasicSettingsPage->m_psp.pfnCallback)(hwnd, uMsg, (LPPROPSHEETPAGE)(&(pBasicSettingsPage->m_psp)));
  }

  if (uMsg == PSPCB_RELEASE) {
    TRACE(_T("LaunchPad: automatically deleting CBasicSettingsPage from heap\n"));
    delete pBasicSettingsPage;
  }

  return retVal;
}



/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage helper functions

VOID CBasicSettingsPage::SyncGUIData(BOOL bSave) {
  static LPCTSTR T_IDENTITY_MAP = _T("identity.map");
  static LPCTSTR T_MT2GM_MAP = _T("mt2gm.map");

  static LPCTSTR T_JOY1_MAP = _T("joy1.map");
  static LPCTSTR T_JOY2_MAP = _T("joy2.map");
  static LPCTSTR T_JOY3_MAP = _T("joy3.map");

  using namespace LaunchPadSettingsHelper;

  //
  // Synchronize the editable controls (checkboxes and radio buttons)
  //  with the settings they represent
  //

  // Compatibility
  SyncCheckBox(bSave, m_settings, _T("winnt.memory"), _T("useEMS"), m_chkEms, FALSE);
  SyncCheckBox(bSave, m_settings, _T("winnt.video"), _T("useVESA"), m_chkVesa, FALSE);
  SyncCheckBox(bSave, m_settings, _T("winnt.storage"), _T("useCDROM"), m_chkCdrom, FALSE);

  // MIDI
  SyncRadioButton(bSave, m_settings, _T("vdms.midi"), _T("mapFile"), m_optGmidi, TRUE, T_IDENTITY_MAP);
  SyncRadioButton(bSave, m_settings, _T("vdms.midi"), _T("mapFile"), m_optMt32, FALSE, T_MT2GM_MAP);
  SyncRadioButton(bSave, m_settings, _T("vdms.midi"), _T("mapFile"), m_optMidiother, T_IDENTITY_MAP, T_MT2GM_MAP, NULL);

  // Joystick
  SyncRadioButton(bSave, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoy2but, TRUE, T_JOY2_MAP);
  SyncRadioButton(bSave, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoy4but, FALSE, T_JOY3_MAP);
  SyncRadioButton(bSave, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoyother, T_JOY2_MAP, T_JOY3_MAP, NULL);

  // Other
  SyncCheckBox(bSave, m_settings, _T("winnt.dosbox"), _T("exitClose"), m_chkClose, FALSE);

  //
  // Synchronize the read-only controls with the settings they represent
  //
  if (!bSave) {
    // Program icon
    LoadIconCtl(m_settings, _T("program"), _T("icon"), m_icoApp);

    // Program command line
    CString progExec, progParams, progWDir;
    BOOL isIndExec, isIndParams, isIndWDir;

    m_settings.GetValue(_T("program"), _T("executable"), progExec, &isIndExec);
    m_settings.GetValue(_T("program"), _T("params"), progParams, &isIndParams);
    m_settings.GetValue(_T("program"), _T("workdir"), progWDir, &isIndWDir);

    if (isIndExec || isIndParams) {
      m_edtDoscmd.EnableWindow(FALSE);
      m_edtDoscmd.SetWindowText(_T("(multiple values)"));
    } else {
      CString value = GetRelativePath(progExec, FALSE, progWDir) + _T(" ") + progParams;
      value.TrimLeft(); value.TrimRight();

      m_edtDoscmd.EnableWindow(TRUE);
      m_edtDoscmd.SetWindowText(value);
    }
  }
}



/////////////////////////////////////////////////////////////////////////////



void CBasicSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicSettingsPage)
	DDX_Control(pDX, IDC_ICO_APP, m_icoApp);
	DDX_Control(pDX, IDC_EDT_DOSCMD, m_edtDoscmd);
	DDX_Control(pDX, IDC_CHK_EMS, m_chkEms);
	DDX_Control(pDX, IDC_CHK_VESA, m_chkVesa);
	DDX_Control(pDX, IDC_CHK_CDROM, m_chkCdrom);
	DDX_Control(pDX, IDC_OPT_GMIDI, m_optGmidi);
	DDX_Control(pDX, IDC_OPT_MT32, m_optMt32);
	DDX_Control(pDX, IDC_OPT_MIDIOTHER, m_optMidiother);
	DDX_Control(pDX, IDC_OPT_JOY2BUT , m_optJoy2but);
	DDX_Control(pDX, IDC_OPT_JOY4BUT, m_optJoy4but);
	DDX_Control(pDX, IDC_OPT_JOYOTHER, m_optJoyother);
	DDX_Control(pDX, IDC_CHK_CLOSE, m_chkClose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicSettingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBasicSettingsPage)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BUT_CHANGE, OnButChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage message handlers

BOOL CBasicSettingsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

  SyncGUIData(FALSE);

  m_edtDoscmd.SetFocus();

	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBasicSettingsPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();

	// Destroy the icon (if any)
  m_icoApp.DeleteIcon();
}

BOOL CBasicSettingsPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}


BOOL CBasicSettingsPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  ASSERT(::GetDlgItem(m_hWnd, LOWORD(wParam)) == (HWND)lParam);

  if ((LOWORD(wParam) != IDC_EDT_DOSCMD) &&
      (LOWORD(wParam) != IDC_BUT_CHANGE) &&
      (LOWORD(wParam) != IDC_BUT_ADVANCED))
  {
    SetModified();
  }

  return CPropertyPage::OnCommand(wParam, lParam);
}

void CBasicSettingsPage::OnButChange() 
{
	CBasicBrowseDlg dlgBrowse;

  SyncGUIData(TRUE);        // save all changes that occured in the GUI

  m_settings.GetValue(_T("program"), _T("executable"), dlgBrowse.m_edtFile_val);
  m_settings.GetValue(_T("program"), _T("params"), dlgBrowse.m_edtArgs_val);
  m_settings.GetValue(_T("program"), _T("workdir"), dlgBrowse.m_edtDir_val);

  m_settings.GetValue(_T("program"), _T("icon"), dlgBrowse.m_iconLocation);

  switch (dlgBrowse.DoModal()) {
    case IDOK:
      m_settings.SetValue(_T("program"), _T("executable"), dlgBrowse.m_edtFile_val);
      m_settings.SetValue(_T("program"), _T("params"), dlgBrowse.m_edtArgs_val);
      m_settings.SetValue(_T("program"), _T("workdir"), dlgBrowse.m_edtDir_val);

      m_settings.SetValue(_T("program"), _T("icon"), dlgBrowse.m_iconLocation);

      SyncGUIData(FALSE);   // update the GUI to reflect any changed settings
      break;

    case IDCANCEL:
      break;

    default:
      MessageBox(_T("An unknown error has occured"), _T("Error"), MB_OK | MB_ICONERROR);
  }
}