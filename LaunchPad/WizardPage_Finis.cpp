// WizardPage_Finis.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"
#include "WizardPage_Finis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MAX_RETRIES 128       // how many times to retry creation of a file

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Finis property page

CWizardPage_Finis::CWizardPage_Finis(WizardContainer& wizard)
 : CPropertyPageEx(CWizardPage_Finis::IDD), m_wizard(wizard)
{
	//{{AFX_DATA_INIT(CWizardPage_Finis)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWizardPage_Finis::~CWizardPage_Finis()
{
}

void CWizardPage_Finis::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPage_Finis)
	DDX_Control(pDX, IDC_CHK_DESKTOPSHORT, m_chkDesktopshort);
	DDX_Control(pDX, IDC_CHK_SAVESETTINGS, m_chkSavesettings);
	DDX_Control(pDX, IDC_TXT_WIZARDTITLE, m_txtWizardtitle);
	//}}AFX_DATA_MAP
}

void CWizardPage_Finis::SyncWizButtons(void) {
  ::PropSheet_SetWizButtons(GetParent()->GetSafeHwnd(), PSWIZB_BACK | PSWIZB_FINISH);
}

HRESULT CWizardPage_Finis::MakeDesktopLink(LPCTSTR baseName, CString& result) {
  HRESULT hr;
  HANDLE hFile = INVALID_HANDLE_VALUE;
  CString desktopPath, linkName;

  result.Empty();

  if (FAILED(hr = VLPUtil::GetSpecialFolderPath(desktopPath, CSIDL_DESKTOPDIRECTORY)))
    return hr;

  ASSERT(VLPUtil::FileExists(desktopPath));

  for (int i = 0; i < MAX_RETRIES; i++) {
    if (i == 0) {
      linkName.Format(_T("Shortcut to %s"), baseName);
    } else {
      linkName.Format(_T("Shortcut (%d) to %s"), i, baseName);
    }

    result = VLPUtil::GetAbsolutePath(linkName, desktopPath);

    if ((hFile = CreateFile(result, GENERIC_WRITE, FILE_SHARE_DELETE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
      break;
  }

  if (hFile == INVALID_HANDLE_VALUE)
    return HRESULT_FROM_WIN32(GetLastError());

  CloseHandle(hFile);

  return S_OK;
}


BEGIN_MESSAGE_MAP(CWizardPage_Finis, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWizardPage_Finis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Finis message handlers

BOOL CWizardPage_Finis::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();

  // Set up any special fonts
  if (m_fntWizardtitle.CreateFont(25, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_SWISS, _T("Verdana"))) {
    m_txtWizardtitle.SetFont(&m_fntWizardtitle);
  }

  CFont* pFntTmp = m_chkSavesettings.GetFont();
  LOGFONT logFont;

  if ((pFntTmp != NULL) && (pFntTmp->GetLogFont(&logFont) != 0)) {
    logFont.lfWeight = FW_BOLD;

    if (m_fntBoldButtons.CreateFontIndirect(&logFont)) {
      m_chkSavesettings.SetFont(&m_fntBoldButtons);
      m_chkDesktopshort.SetFont(&m_fntBoldButtons);
    }
  }

  // Set up the default values
  m_chkSavesettings.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardPage_Finis::OnSetActive() 
{
  // Set up the wizard buttons
  SyncWizButtons();

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWizardPage_Finis::OnWizardNext() 
{
  ASSERT(m_wizard.history.GetCount() > 0);

	ASSERT(FALSE);  // not supposed to be any "next"

	return -1;
}

LRESULT CWizardPage_Finis::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

  ASSERT(m_wizard.history.GetCount() > 0);

	return m_wizard.history.RemoveTail();
}

BOOL CWizardPage_Finis::OnWizardFinish() 
{
  HRESULT hr;

  BOOL bIgnoreError;

  BOOL bSaveSettings = (m_chkSavesettings.GetCheck() != BST_UNCHECKED);
  BOOL bDesktopShort = (m_chkDesktopshort.GetCheck() != BST_UNCHECKED);

  // Save a shortcut in the application's folder
  if (bSaveSettings) {
    while (FAILED(hr = m_wizard.settings.Commit())) {
      CString strTmp1, strTmp2;
      strTmp1.FormatMessage(IDS_MSG_COMMITERR, hr, (LPCTSTR)VLPUtil::FormatMessage(hr, true, NULL), (LPCTSTR)(m_wizard.settings.GetFileNames()));
      GetWindowText(strTmp2);

      bIgnoreError = FALSE;

      switch (MessageBox(strTmp1, strTmp2, MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONWARNING)) {
        case IDABORT:
          return FALSE;         // return to wizard
        case IDRETRY:
          break;                // try again
        case IDIGNORE:
          bIgnoreError = TRUE;  // continue
      }

      if (bIgnoreError)
        break;
    }
  }

  // Save a shortcut on the desktop
  if (bDesktopShort) {
    CString shortcutFileName;

    while (FAILED(hr = MakeDesktopLink(VLPUtil::GetFilename(m_wizard.exeFileName, TRUE) + _T(".vlp"), shortcutFileName))) {
      CString strTmp1, strTmp2;
      strTmp1.FormatMessage(IDS_MSG_SHORTCUTERR, (LPCTSTR)shortcutFileName, hr, (LPCTSTR)VLPUtil::FormatMessage(hr, true, NULL));
      GetWindowText(strTmp2);

      bIgnoreError = FALSE;

      switch (MessageBox(strTmp1, strTmp2, MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONWARNING)) {
        case IDABORT:
          return FALSE;         // return to wizard
        case IDRETRY:
          break;                // try again
        case IDIGNORE:
          bIgnoreError = TRUE;  // continue
      }

      if (bIgnoreError)
        break;
    }

    CLaunchPadSettings desktopSettings(shortcutFileName);
    desktopSettings.LoadFromTemplate(m_wizard.settings);

    while (FAILED(hr = desktopSettings.Commit())) {
      CString strTmp1, strTmp2;
      strTmp1.FormatMessage(IDS_MSG_COMMITERR, hr, (LPCTSTR)VLPUtil::FormatMessage(hr, true, NULL), (LPCTSTR)(desktopSettings.GetFileNames(_T(", "), 2)));
      GetWindowText(strTmp2);

      bIgnoreError = FALSE;

      switch (MessageBox(strTmp1, strTmp2, MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONWARNING)) {
        case IDABORT:
          return FALSE;         // return to wizard
        case IDRETRY:
          break;                // try again
        case IDIGNORE:
          bIgnoreError = TRUE;  // continue
      }

      if (bIgnoreError)
        break;
    }
  }

	return CPropertyPageEx::OnWizardFinish();
}
