// WizardSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet

CWizardSheet::CWizardSheet(LPCTSTR exeFileName, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
 : CPropertySheetEx(nIDCaption, pParentWnd, iSelectPage),
   m_wizard(exeFileName),
   m_p1(m_wizard), m_p2(m_wizard), m_p3(m_wizard), m_p4(m_wizard)
{
  Init();
}

CWizardSheet::CWizardSheet(LPCTSTR exeFileName, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
 : CPropertySheetEx(pszCaption, pParentWnd, iSelectPage),
   m_wizard(exeFileName),
   m_p1(m_wizard), m_p2(m_wizard), m_p3(m_wizard), m_p4(m_wizard)
{
  Init();
}

CWizardSheet::~CWizardSheet()
{
}

void CWizardSheet::Init(void) {
  m_psh.hInstance = AfxGetInstanceHandle();                   // Set instance, for the bitmap resources
  m_psh.dwFlags |= PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER; // Wizard97 with watermark and header
  m_psh.dwFlags &= ~PSH_USEHBMWATERMARK & ~PSH_USEHBMHEADER;  // Give resource ID (not handle) to bitmap
  m_psh.pszbmWatermark = MAKEINTRESOURCE(GetScreenBpp() < 8 ? IDB_WIZ_BACK_16 : IDB_WIZ_BACK_256);
  m_psh.pszbmHeader    = MAKEINTRESOURCE(GetScreenBpp() < 8 ? IDB_WIZ_HEAD_16 : IDB_WIZ_HEAD_256);

  AddPage(&m_p1);
  AddPage(&m_p2);
  AddPage(&m_p3);
  AddPage(&m_p4);
}

int CWizardSheet::GetScreenBpp(void) {
  int bpp = -1;
  HDC hdc = ::GetDC(NULL);

  if (hdc != NULL) {
    bpp = ::GetDeviceCaps(hdc, BITSPIXEL);
    ::ReleaseDC(NULL, hdc);
	}

  return bpp;
}


BEGIN_MESSAGE_MAP(CWizardSheet, CPropertySheetEx)
	//{{AFX_MSG_MAP(CWizardSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet message handlers

int CWizardSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CPropertySheetEx::OnCreate(lpCreateStruct) == -1)
    return -1;

	// Register ourselves as a drop target
  if (!m_dropTarget.Register(this))
    return -1;

  return 0;
}
