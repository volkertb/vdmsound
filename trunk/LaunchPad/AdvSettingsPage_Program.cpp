// AdvSettingsPage_Program.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Program.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Program property page

CAdvSettingsPage_Program::CAdvSettingsPage_Program(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Program::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Program)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Program::~CAdvSettingsPage_Program()
{
}

void CAdvSettingsPage_Program::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Program)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Program, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Program)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Program message handlers

BOOL CAdvSettingsPage_Program::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
