// AdvSettingsPage_Midi.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsPage_Midi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi property page

CAdvSettingsPage_Midi::CAdvSettingsPage_Midi(CLaunchPadSettings& settings)
 : CPropertyPage(CAdvSettingsPage_Midi::IDD), m_settings(settings)
{
	//{{AFX_DATA_INIT(CAdvSettingsPage_Midi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvSettingsPage_Midi::~CAdvSettingsPage_Midi()
{
}

void CAdvSettingsPage_Midi::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettingsPage_Midi)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettingsPage_Midi, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvSettingsPage_Midi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi message handlers

BOOL CAdvSettingsPage_Midi::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
