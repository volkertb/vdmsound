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
  : CDialog(CChangeIconDlg::IDD, pParent), m_lastEdtFile(_T(""))
{
	//{{AFX_DATA_INIT(CChangeIconDlg)
	m_edtFile_val = _T("");
	m_lstIcons_val = -1;
	//}}AFX_DATA_INIT
}

BOOL CChangeIconDlg::UpdateIconList(void) {
  BOOL retVal = TRUE;
  CString tmpStr1, tmpStr2;

  UpdateData(TRUE);

  if (m_edtFile_val.IsEmpty()) {
    m_lstIcons.UnloadIcons();
  } else if (m_edtFile_val.CollateNoCase(m_lastEdtFile) != 0) {
    CString iconPath;
    int iconIndex;
    VLPUtil::ParseIconLocation(m_edtFile_val, iconPath, iconIndex);

    m_edtFile_val = iconPath;
    m_lastEdtFile = m_edtFile_val;

    switch (m_lstIcons.LoadIcons(iconPath)) {
      case -1:
        tmpStr1.FormatMessage(IDS_MSG_ICONERROR, GetLastError(), (LPCTSTR)iconPath);
        GetWindowText(tmpStr2);
        MessageBox(tmpStr1, tmpStr2, MB_OK | MB_ICONWARNING);
        retVal = FALSE;
        break;

      case 0:
        tmpStr1.FormatMessage(IDS_MSG_NOICONS, (LPCTSTR)iconPath);
        GetWindowText(tmpStr2);
        MessageBox(tmpStr1, tmpStr2, MB_OK | MB_ICONWARNING);
        retVal = FALSE;
        break;

      default:
        // Do nothing
        break;
    }

    m_lstIcons_val = iconIndex;

    UpdateData(FALSE);
  }

  return retVal;
}


void CChangeIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeIconDlg)
	DDX_Control(pDX, IDC_LST_ICONS, m_lstIcons);
	DDX_Text(pDX, IDC_EDT_FILE, m_edtFile_val);
	DDV_MaxChars(pDX, m_edtFile_val, 79);
	DDX_LBIndex(pDX, IDC_LST_ICONS, m_lstIcons_val);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeIconDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeIconDlg)
	ON_LBN_SETFOCUS(IDC_LST_ICONS, OnSetfocusLstIcons)
	ON_LBN_DBLCLK(IDC_LST_ICONS, OnDblclkLstIcons)
	ON_BN_CLICKED(IDC_BUT_FILEBROWSE, OnButFilebrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg message handlers

BOOL CChangeIconDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  UpdateIconList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeIconDlg::OnSetfocusLstIcons() 
{
  if (!UpdateIconList()) {
    GotoDlgCtrl(GetDlgItem(IDC_EDT_FILE));
  }
}

void CChangeIconDlg::OnDblclkLstIcons() 
{
  if (UpdateData(TRUE)) {
    EndDialog(IDOK);
  }
}

void CChangeIconDlg::OnButFilebrowse() 
{
  CString strFilter;
  strFilter.LoadString(IDS_TXT_FILTER2);
  strFilter += _T('|');

  UpdateData(TRUE);     // save all changes that occured in the GUI

  CFileDialog dlgFile(TRUE, NULL, m_edtFile_val, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, strFilter, this);

  if (dlgFile.DoModal() == IDOK) {
    m_edtFile_val = dlgFile.GetPathName();
    UpdateData(FALSE);  // update the GUI to reflect any changed settings
    UpdateIconList();
  }
}

void CChangeIconDlg::OnOK() 
{
  if (UpdateIconList()) {
  	CDialog::OnOK();
  }
}
