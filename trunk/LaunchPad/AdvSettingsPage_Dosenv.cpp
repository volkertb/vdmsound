// AdvSettingsPage_Dosenv.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Dosenv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Dosenv property page

CAdvSettingsPage_Dosenv::CAdvSettingsPage_Dosenv(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Dosenv::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Dosenv)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Dosenv::~CAdvSettingsPage_Dosenv()
{
}

void CAdvSettingsPage_Dosenv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Dosenv)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Dosenv, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Dosenv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Dosenv message handlers

BOOL CAdvSettingsPage_Dosenv::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
