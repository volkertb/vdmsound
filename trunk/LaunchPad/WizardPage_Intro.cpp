// WizardPage_Intro.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"
#include "WizardPage_Intro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Intro property page

CWizardPage_Intro::CWizardPage_Intro(WizardContainer& wizard)
 : CPropertyPageEx(CWizardPage_Intro::IDD), m_wizard(wizard)
{
	//{{AFX_DATA_INIT(CWizardPage_Intro)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWizardPage_Intro::~CWizardPage_Intro()
{
}

void CWizardPage_Intro::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPage_Intro)
	DDX_Control(pDX, IDC_OPT_USEDEFAULT, m_optUsedefault);
	DDX_Control(pDX, IDC_OPT_USECUSTOM, m_optUsecustom);
	DDX_Control(pDX, IDC_TXT_WIZARDTITLE, m_txtWizardtitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPage_Intro, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWizardPage_Intro)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Intro message handlers

BOOL CWizardPage_Intro::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();

  // Set up any special fonts
  if (m_fntWizardtitle.CreateFont(25, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_SWISS, _T("Verdana"))) {
    m_txtWizardtitle.SetFont(&m_fntWizardtitle);
  }

  CFont* pFntTmp = m_optUsedefault.GetFont();
  LOGFONT logFont;

  if ((pFntTmp != NULL) && (pFntTmp->GetLogFont(&logFont) != 0)) {
    logFont.lfWeight = FW_BOLD;

    if (m_fntBoldButtons.CreateFontIndirect(&logFont)) {
      m_optUsedefault.SetFont(&m_fntBoldButtons);
      m_optUsecustom.SetFont(&m_fntBoldButtons);
    }
  }

  // Set up the default values
  m_optUsedefault.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardPage_Intro::OnSetActive() 
{
  // Set up the wizard buttons
  ::PropSheet_SetWizButtons(GetParent()->GetSafeHwnd(), PSWIZB_NEXT);

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWizardPage_Intro::OnWizardNext() 
{
  LRESULT retVal;
  BOOL bUseDefault = (m_optUsedefault.GetCheck() != BST_UNCHECKED);

  if (bUseDefault) {
    // Load in the settings from %VDMSPath%\default.vlp
    TRACE(_T("CWizardPage_Intro::OnWizardNext() - Loading in default settings\n"));
    m_wizard.settings.Copy(CLaunchPadSettings(VLPUtil::GetVDMSFilePath(_T("default.vlp"))));

    // Advance to the "Finish" page
    retVal = IDD_WIZPROPPAGE_FINIS;
  } else {
    // Advance to the "Customize Configuration" page
    retVal = IDD_WIZPROPPAGE_CUSTOM_1;
  }

  ASSERT(m_wizard.history.GetCount() == 0);
  m_wizard.history.AddTail(IDD);

	return retVal;
}

LRESULT CWizardPage_Intro::OnWizardBack() 
{
  ASSERT(m_wizard.history.GetCount() == 0);

	ASSERT(FALSE);  // not supposed to be any "back"

	return -1;
}
