// AdvSettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AdvSettingsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsSheet

CAdvSettingsSheet::CAdvSettingsSheet(CLaunchPadSettings& settings, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
 : CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
   CSettingsContainer(settings),
   m_p1(m_settings), m_p2(m_settings), m_p3(m_settings), m_p4(m_settings), m_p5(m_settings), m_p6(m_settings), m_p7(m_settings)
{
  Init();
}

CAdvSettingsSheet::CAdvSettingsSheet(CLaunchPadSettings& settings, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
 : CPropertySheet(pszCaption, pParentWnd, iSelectPage),
   CSettingsContainer(settings),
   m_p1(m_settings), m_p2(m_settings), m_p3(m_settings), m_p4(m_settings), m_p5(m_settings), m_p6(m_settings), m_p7(m_settings)
{
  Init();
}

CAdvSettingsSheet::~CAdvSettingsSheet()
{
}

void CAdvSettingsSheet::Init(void) {
  AddPage(&m_p1);
  AddPage(&m_p2);
  AddPage(&m_p3);
  AddPage(&m_p4);
  AddPage(&m_p5);
  AddPage(&m_p6);
  AddPage(&m_p7);
}


BEGIN_MESSAGE_MAP(CAdvSettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CAdvSettingsSheet)
	ON_MESSAGE(WM_SAVESETTINGS, OnSaveSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsSheet message handlers

BOOL CAdvSettingsSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
  // Enable context help
  m_help.Attach(this);

	return bResult;
}

LRESULT CAdvSettingsSheet::OnSaveSettings(WPARAM wParam, LPARAM lParam)
{
  Save();

  return 0;
}
