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
	m_edtDosargs_val = _T("");
	m_edtDosprogram_val = _T("");
	m_edtDosdir_val = _T("");
	//}}AFX_DATA_INIT
}


void CBasicBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicBrowseDlg)
	DDX_Control(pDX, IDC_EDT_DOSARGS, m_edtDosargs);
	DDX_Control(pDX, IDC_EDT_DOSDIR, m_edtDosdir);
	DDX_Control(pDX, IDC_EDT_DOSPROGRAM, m_edtDosprogram);
	DDX_Text(pDX, IDC_EDT_DOSARGS, m_edtDosargs_val);
	DDV_MaxChars(pDX, m_edtDosargs_val, 63);
	DDX_Text(pDX, IDC_EDT_DOSPROGRAM, m_edtDosprogram_val);
	DDV_MaxChars(pDX, m_edtDosprogram_val, 63);
	DDX_Text(pDX, IDC_EDT_DOSDIR, m_edtDosdir_val);
	DDV_MaxChars(pDX, m_edtDosdir_val, 63);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CBasicBrowseDlg)
	ON_BN_CLICKED(IDC_BUT_DOSPROGRAMBROWSE, OnButDosprogrambrowse)
	ON_BN_CLICKED(IDC_BUT_CHANGEICON, OnButChangeicon)
	ON_EN_UPDATE(IDC_EDT_DOSPROGRAM, OnUpdateEdtdosprogram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg message handlers

BOOL CBasicBrowseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Enable context help
  m_help.Attach(this);

  // Enable auto-complete for file names
  VLPUtil::EnableAutoComplete(m_edtDosprogram.GetSafeHwnd());

  // Register drag-n-drop handlers
  m_edtDosargs.Register();
  m_edtDosdir.Register();
  m_edtDosprogram.Register();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBasicBrowseDlg::OnButDosprogrambrowse() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  UpdateData(TRUE);     // save all changes that occured in the GUI
	COpenDOSProgramDialog dlgFile(m_edtDosprogram_val, this);

  switch (dlgFile.DoModal()) {
    case IDOK:
      m_edtDosprogram_val = dlgFile.GetPathName();
      m_edtDosdir_val     = VLPUtil::GetDirectory(m_edtDosprogram_val);
      UpdateData(FALSE);  // update the GUI to reflect any changed settings
      break;

    case IDCANCEL:
      break;

    default:
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
  }
}

void CBasicBrowseDlg::OnButChangeicon() 
{
  CWaitCursor wait;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;
  CChangeIconDlg dlgIcon(this);

  dlgIcon.m_edtIcofile_val = m_iconLocation;

  switch (dlgIcon.DoModal()) {
    case IDOK:
      m_iconLocation.Format(_T("%s,%d"), dlgIcon.m_edtIcofile_val, max(0, dlgIcon.m_lstIcons_val));
      break;

    case IDCANCEL:
      break;

    default:
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      GetWindowText(strTmp2);
      MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONERROR);
  }
}

void CBasicBrowseDlg::OnUpdateEdtdosprogram() 
{
  CString strDir;
  m_edtDosprogram.GetWindowText(strDir);
  m_edtDosdir.SetWindowText(VLPUtil::GetDirectory(strDir));
}
