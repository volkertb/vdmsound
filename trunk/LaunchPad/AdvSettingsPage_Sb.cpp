// AdvSettingsPage_Sb.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Sb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb property page

CAdvSettingsPage_Sb::CAdvSettingsPage_Sb(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Sb::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Sb)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Sb::~CAdvSettingsPage_Sb()
{
}

void CAdvSettingsPage_Sb::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Sb)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Sb, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Sb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Sb message handlers

BOOL CAdvSettingsPage_Sb::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
