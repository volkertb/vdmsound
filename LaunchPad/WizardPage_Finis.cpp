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
  ::PropSheet_SetWizButtons(GetParent()->GetSafeHwnd(), PSWIZB_BACK | PSWIZB_FINISH);
	
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
