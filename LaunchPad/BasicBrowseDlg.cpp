// BasicBrowseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BasicBrowseDlg.h"

#include "ChangeIconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg dialog


CBasicBrowseDlg::CBasicBrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBasicBrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBasicBrowseDlg)
	m_edtArgs_val = _T("");
	m_edtFile_val = _T("");
	m_edtDir_val = _T("");
	//}}AFX_DATA_INIT
}


void CBasicBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicBrowseDlg)
	DDX_Control(pDX, IDC_EDT_DIR, m_edtDir);
	DDX_Control(pDX, IDC_EDT_FILE, m_edtFile);
	DDX_Text(pDX, IDC_EDT_ARGS, m_edtArgs_val);
	DDV_MaxChars(pDX, m_edtArgs_val, 63);
	DDX_Text(pDX, IDC_EDT_FILE, m_edtFile_val);
	DDV_MaxChars(pDX, m_edtFile_val, 63);
	DDX_Text(pDX, IDC_EDT_DIR, m_edtDir_val);
	DDV_MaxChars(pDX, m_edtDir_val, 63);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CBasicBrowseDlg)
	ON_BN_CLICKED(IDC_BUT_FILEBROWSE, OnButFilebrowse)
	ON_BN_CLICKED(IDC_BUT_CHANGEICON, OnButChangeicon)
	ON_EN_UPDATE(IDC_EDT_FILE, OnUpdateEdtFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg message handlers

BOOL CBasicBrowseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  VLPUtil::EnableAutoComplete(m_edtFile.GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBasicBrowseDlg::OnButFilebrowse() 
{
  UpdateData(TRUE);     // save all changes that occured in the GUI
	COpenDOSProgramDialog dlgFile(m_edtFile_val, this);

  if (dlgFile.DoModal() == IDOK) {
    m_edtFile_val = dlgFile.GetPathName();
    m_edtDir_val = VLPUtil::GetDirectory(m_edtFile_val);
    UpdateData(FALSE);  // update the GUI to reflect any changed settings
  }
}

void CBasicBrowseDlg::OnButChangeicon() 
{
  CChangeIconDlg dlgIcon(this);

  dlgIcon.m_edtFile_val = m_iconLocation;

  if (dlgIcon.DoModal() == IDOK) {
    m_iconLocation.Format(_T("%s,%d"), dlgIcon.m_edtFile_val, max(0, dlgIcon.m_lstIcons_val));
  }
}

void CBasicBrowseDlg::OnUpdateEdtFile() 
{
  CString strDir;
  m_edtFile.GetWindowText(strDir);
  m_edtDir.SetWindowText(VLPUtil::GetDirectory(strDir));
}
