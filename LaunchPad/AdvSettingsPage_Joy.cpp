// AdvSettingsPage_Joy.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Joy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Joy property page

CAdvSettingsPage_Joy::CAdvSettingsPage_Joy(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Joy::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Joy)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Joy::~CAdvSettingsPage_Joy()
{
}

void CAdvSettingsPage_Joy::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Joy)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Joy, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Joy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Joy message handlers

BOOL CAdvSettingsPage_Joy::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
