// BasicSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BasicSettingsPage.h"

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
		// NOTE: the ClassWizard will add member initialization here
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

  pBasicSettingsPage->m_psp.dwFlags |= PSP_USEICONID;
  pBasicSettingsPage->m_psp.pszIcon = (LPCSTR)MAKEINTRESOURCE(IDI_ICON);

  // Obtain the Win32 property sheet handle
  HPROPSHEETPAGE hPage = CreatePropertySheetPage((PROPSHEETPAGE*)(&(pBasicSettingsPage->m_psp)));

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

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage helper functions

VOID CBasicSettingsPage::SyncGUIData(BOOL isSetOp) {
  static LPCTSTR T_IDENTITY_MAP = _T("identity.map");
  static LPCTSTR T_MT2GM_MAP = _T("mt2gm.map");

  using namespace LaunchPadSettingsHelper;

  SyncIcon(isSetOp, m_settings, _T("program"), _T("icon"), m_icoApp);

  SyncCheckBox(isSetOp, m_settings, _T("winnt.memory"), _T("useEMS"), m_chkEms, FALSE);
  SyncCheckBox(isSetOp, m_settings, _T("winnt.video"), _T("useVESA"), m_chkVesa, FALSE);
  SyncCheckBox(isSetOp, m_settings, _T("winnt.storage"), _T("useCDROM"), m_chkCdrom, FALSE);

  SyncRadioButton(isSetOp, m_settings, _T("vdms.midi"), _T("mapFile"), m_optGmidi, TRUE, T_IDENTITY_MAP);
  SyncRadioButton(isSetOp, m_settings, _T("vdms.midi"), _T("mapFile"), m_optMt32, FALSE, T_MT2GM_MAP);
  SyncRadioButton(isSetOp, m_settings, _T("vdms.midi"), _T("mapFile"), m_optMidiother, T_IDENTITY_MAP, T_MT2GM_MAP, NULL);

  SyncRadioButton(isSetOp, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoy2but, TRUE, T_IDENTITY_MAP);
  SyncRadioButton(isSetOp, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoy4but, FALSE, T_MT2GM_MAP);
  SyncRadioButton(isSetOp, m_settings, _T("vdms.gameport"), _T("mapFile"), m_optJoyother, T_IDENTITY_MAP, T_MT2GM_MAP, NULL);

  SyncCheckBox(isSetOp, m_settings, _T("winnt.dosbox"), _T("exitClose"), m_chkClose, FALSE);

  m_edtDoscmd.SetWindowText(_T("h:\\work\\vdmstest\\kir2\\hof.exe /?"));
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
	ON_WM_HELPINFO()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage message handlers

BOOL CBasicSettingsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

  SyncGUIData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBasicSettingsPage::PostNcDestroy() 
{
	CPropertyPage::PostNcDestroy();

  // Automatically free up the memory associated with
  //  this MFC object when the window is destroyed
  ASSERT_VALID(this);
  TRACE(_T("LaunchPad: automatically deleting CBasicSettingsPage from heap\n"));
  delete this;
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
