// BasicSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BasicSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage property page

IMPLEMENT_DYNCREATE(CBasicSettingsPage, CPropertyPage)

CBasicSettingsPage::CBasicSettingsPage() : CPropertyPage(CBasicSettingsPage::IDD)
{
	//{{AFX_DATA_INIT(CBasicSettingsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBasicSettingsPage::~CBasicSettingsPage()
{
}

//
// Adds the property page to a property sheet using the supplied
//  callback function and long value
//
HRESULT CBasicSettingsPage::AddPage(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam, const CStringArray& fileNames) {
  // Create a self-destroying instance of CBasicSettingsPage
  CBasicSettingsPage* pBasicSettingsPage = new CBasicSettingsPage();

  ASSERT_VALID(pBasicSettingsPage);

  if (!pBasicSettingsPage)
    return E_OUTOFMEMORY;

  pBasicSettingsPage->m_psp.hIcon = 0;

  // Obtain the Win32 property sheet handle
  HPROPSHEETPAGE hPage = CreatePropertySheetPage((PROPSHEETPAGE*)(&(pBasicSettingsPage->m_psp)));

  if (!hPage) {
    delete pBasicSettingsPage;
    return HRESULT_FROM_WIN32(GetLastError());
  }

  // Call the callback function to add the page to a property sheet
  if (!lpfnAddPageProc(hPage, lParam)) {
    DestroyPropertySheetPage(hPage);
    delete pBasicSettingsPage;
    return E_UNEXPECTED;
  }

  // Finish pBasicSettingsPage's initialization
  pBasicSettingsPage->m_fileNames.Copy(fileNames);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////



void CBasicSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicSettingsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicSettingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBasicSettingsPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage message handlers

void CBasicSettingsPage::PostNcDestroy() 
{
	CPropertyPage::PostNcDestroy();

  // Automatically free up the memory associated with
  //  this MFC object when the window is destroyed
  ASSERT_VALID(this);
  TRACE("LaunchPad: automatically deleting CBasicSettingsPage from heap\n");
  delete this;
}
