// ChangeIconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ChangeIconDlg.h"

#include "LaunchPadUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg dialog


CChangeIconDlg::CChangeIconDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeIconDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeIconDlg)
	m_edtFile_val = _T("");
	//}}AFX_DATA_INIT
}

void CChangeIconDlg::UpdateIconList(void) {
  using namespace LaunchPadSettingsHelper;

  UpdateData(TRUE);

  CString iconPath;
  int iconIndex;
  ParseIconLocation(m_edtFile_val, iconPath, iconIndex);
  m_lstIcons.LoadIcons(iconPath, iconIndex);
}


void CChangeIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeIconDlg)
	DDX_Control(pDX, IDC_LST_ICONS, m_lstIcons);
	DDX_Text(pDX, IDC_EDT_FILE, m_edtFile_val);
	DDV_MaxChars(pDX, m_edtFile_val, 80);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeIconDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeIconDlg)
	ON_EN_KILLFOCUS(IDC_EDT_FILE, OnKillfocusEdtFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg message handlers

BOOL CChangeIconDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeIconDlg::OnKillfocusEdtFile() 
{
  UpdateIconList();
}
