// AdvSettingsPage_Adlib.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Adlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Adlib property page

CAdvSettingsPage_Adlib::CAdvSettingsPage_Adlib(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Adlib::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Adlib)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Adlib::~CAdvSettingsPage_Adlib()
{
}

void CAdvSettingsPage_Adlib::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Adlib)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Adlib, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Adlib)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Adlib message handlers

BOOL CAdvSettingsPage_Adlib::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
