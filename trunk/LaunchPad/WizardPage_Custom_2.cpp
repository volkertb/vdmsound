// WizardPage_Custom_2.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"
#include "WizardPage_Custom_2.h"

#include "AdvSettingsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_2 property page

CWizardPage_Custom_2::CWizardPage_Custom_2(WizardContainer& wizard)
 : CPropertyPageEx(CWizardPage_Custom_2::IDD), m_wizard(wizard)
{
	//{{AFX_DATA_INIT(CWizardPage_Custom_2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWizardPage_Custom_2::~CWizardPage_Custom_2()
{
}

void CWizardPage_Custom_2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPage_Custom_2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CWizardPage_Custom_2::SyncWizButtons(void) {
  ::PropSheet_SetWizButtons(GetParent()->GetSafeHwnd(), PSWIZB_BACK | PSWIZB_NEXT);
}


BEGIN_MESSAGE_MAP(CWizardPage_Custom_2, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWizardPage_Custom_2)
	ON_BN_CLICKED(IDC_BUT_ADVANCED, OnButAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPage_Custom_2 message handlers

BOOL CWizardPage_Custom_2::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardPage_Custom_2::OnSetActive() 
{
  // Set up the wizard buttons
  SyncWizButtons();

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWizardPage_Custom_2::OnWizardNext() 
{
  LRESULT retVal = IDD_WIZPROPPAGE_FINIS;

  // TODO: Add your specialized code here and/or call the base class

  ASSERT(m_wizard.history.GetCount() > 0);
  m_wizard.history.AddTail(IDD);

	return retVal;
}

LRESULT CWizardPage_Custom_2::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

  ASSERT(m_wizard.history.GetCount() > 0);

	return m_wizard.history.RemoveTail();
}

void CWizardPage_Custom_2::OnButAdvanced() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  CAdvSettingsSheet propSheet(m_wizard.settings, IDS_ADVPROPERTIES, this, 0);

  switch (propSheet.DoModal()) {
    case IDOK:
    case IDCANCEL:          // Settings may have been changed by "Apply" in the property sheet
      break;

    default:
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
  }
}
