// AdvSettingsPage_Compat.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Compat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Compat property page

CAdvSettingsPage_Compat::CAdvSettingsPage_Compat(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Compat::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Compat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Compat::~CAdvSettingsPage_Compat()
{
}

void CAdvSettingsPage_Compat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Compat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Compat, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Compat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Compat message handlers

BOOL CAdvSettingsPage_Compat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
