// WizardPage_Custom_1.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"
#include "WizardPage_Custom_1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_1 property page

CWizardPage_Custom_1::CWizardPage_Custom_1(WizardContainer& wizard)
 : CPropertyPageEx(CWizardPage_Custom_1::IDD), m_wizard(wizard)
{
	//{{AFX_DATA_INIT(CWizardPage_Custom_1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWizardPage_Custom_1::~CWizardPage_Custom_1()
{
}

void CWizardPage_Custom_1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPage_Custom_1)
	DDX_Control(pDX, IDC_BUT_TEMPLATE, m_butTemplate);
	DDX_Control(pDX, IDC_EDT_TEMPLATE, m_edtTemplate);
	DDX_Control(pDX, IDC_OPT_MAKENEW, m_optMakenew);
	DDX_Control(pDX, IDC_OPT_USETEMPLATE, m_optUsetemplate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPage_Custom_1, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWizardPage_Custom_1)
	ON_BN_CLICKED(IDC_OPT_MAKENEW, OnOptClicked)
	ON_BN_CLICKED(IDC_OPT_USETEMPLATE, OnOptClicked)
	ON_BN_CLICKED(IDC_BUT_TEMPLATE, OnButTemplate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_1 message handlers

BOOL CWizardPage_Custom_1::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();
	
  // Set up any special fonts
  CFont* pFntTmp = m_optMakenew.GetFont();
  LOGFONT logFont;

  if ((pFntTmp != NULL) && (pFntTmp->GetLogFont(&logFont) != 0)) {
    logFont.lfWeight = FW_BOLD;

    if (m_fntBoldButtons.CreateFontIndirect(&logFont)) {
      m_optMakenew.SetFont(&m_fntBoldButtons);
      m_optUsetemplate.SetFont(&m_fntBoldButtons);
    }
  }

  // Set up the default values
  m_optMakenew.SetCheck(BST_CHECKED);
  OnOptClicked();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardPage_Custom_1::OnSetActive() 
{
  // Set up the wizard buttons
  ::PropSheet_SetWizButtons(GetParent()->GetSafeHwnd(), PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWizardPage_Custom_1::OnWizardNext() 
{
  LRESULT retVal = IDD_WIZPROPPAGE_CUSTOM_2;
  BOOL bMakeNew = (m_optMakenew.GetCheck() != BST_UNCHECKED);

  if (bMakeNew) {
    TRACE(_T("CWizardPage_Custom_1::OnWizardNext() - Resetting settings\n"));
    m_wizard.settings.Reset();
  } else {
    CString fileName;
    m_edtTemplate.GetWindowText(fileName);

    if (VLPUtil::FileExists(fileName)) {
      TRACE(_T("CWizardPage_Custom_1::OnWizardNext() - Loading in settings from '%s'\n"), (LPCTSTR)fileName);
      m_wizard.settings.Copy(CLaunchPadSettings(fileName));
    } else {
      CString strTmp1, strTmp2;
      strTmp1.FormatMessage(IDS_MSG_NOFILEERR, fileName);
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
      return -1;  //do not advance in the wizard
    }
  }

  ASSERT(m_wizard.history.GetCount() > 0);
  m_wizard.history.AddTail(IDD);

	return retVal;
}

LRESULT CWizardPage_Custom_1::OnWizardBack() 
{
  ASSERT(m_wizard.history.GetCount() > 0);

	return m_wizard.history.RemoveTail();
}

void CWizardPage_Custom_1::OnOptClicked() 
{
  BOOL bMakeNew = (m_optMakenew.GetCheck() != BST_UNCHECKED);

  m_edtTemplate.EnableWindow(!bMakeNew);
  m_butTemplate.EnableWindow(!bMakeNew);
}

void CWizardPage_Custom_1::OnButTemplate() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp;

  CString fileName;
  m_edtTemplate.GetWindowText(fileName);

  CFileDialog dlgFile(TRUE, NULL, fileName.IsEmpty() ? VLPUtil::RenameExtension(m_wizard.exeFileName, _T(".vlp")) : fileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, VLPUtil::LoadString(IDS_TXT_FILTER5), this);

  switch (dlgFile.DoModal()) {
    case IDOK:
      m_edtTemplate.SetWindowText(dlgFile.GetPathName());
      break;

    case IDCANCEL:
      break;

    default:
      lastError = GetLastError();
      strTmp.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      MessageBox(strTmp, NULL, MB_OK | MB_ICONERROR);
  }
}
