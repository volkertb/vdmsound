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
  : CDialog(CChangeIconDlg::IDD, pParent), m_lastEdtIcofile_val(_T(""))
{
	//{{AFX_DATA_INIT(CChangeIconDlg)
	m_edtIcofile_val = _T("");
	m_lstIcons_val = -1;
	//}}AFX_DATA_INIT
}

BOOL CChangeIconDlg::UpdateIconList(void) {
  BOOL retVal = TRUE;
  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  UpdateData(TRUE);

  if (m_edtIcofile_val.IsEmpty()) {
    m_lstIcons.UnloadIcons();
  } else if (m_edtIcofile_val.CollateNoCase(m_lastEdtIcofile_val) != 0) {
    CString iconPath;
    int iconIndex;
    VLPUtil::ParseIconLocation(m_edtIcofile_val, iconPath, iconIndex);

    m_edtIcofile_val     = iconPath;
    m_lastEdtIcofile_val = m_edtIcofile_val;

    switch (m_lstIcons.LoadIcons(iconPath)) {
      case -1:
        lastError = GetLastError();
        strTmp1.FormatMessage(IDS_MSG_ICONERROR, (LPCTSTR)iconPath, (int)lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
        GetWindowText(strTmp2);
        MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONWARNING);
        retVal = FALSE;
        break;

      case 0:
        strTmp1.FormatMessage(IDS_MSG_NOICONS, (LPCTSTR)iconPath);
        GetWindowText(strTmp2);
        MessageBox(strTmp1, strTmp2, MB_OK | MB_ICONWARNING);
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
	DDX_Control(pDX, IDC_EDT_ICOFILE, m_edtIcofile);
	DDX_Text(pDX, IDC_EDT_ICOFILE, m_edtIcofile_val);
	DDV_MaxChars(pDX, m_edtIcofile_val, 79);
	DDX_Control(pDX, IDC_LST_ICONS, m_lstIcons);
	DDX_LBIndex(pDX, IDC_LST_ICONS, m_lstIcons_val);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeIconDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeIconDlg)
	ON_LBN_SETFOCUS(IDC_LST_ICONS, OnSetfocusLstIcons)
	ON_LBN_DBLCLK(IDC_LST_ICONS, OnDblclkLstIcons)
	ON_BN_CLICKED(IDC_BUT_ICOBROWSE, OnButIcobrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg message handlers

BOOL CChangeIconDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  // Enable context help
  m_help.Attach(this);

  // Load list of icons
  UpdateIconList();

  // Enable auto-complete for file names
  VLPUtil::EnableAutoComplete(m_edtIcofile.GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeIconDlg::OnSetfocusLstIcons() 
{
  CWaitCursor wait;

  if (!UpdateIconList()) {
    GotoDlgCtrl(GetDlgItem(IDC_EDT_ICOFILE));
  }
}

void CChangeIconDlg::OnDblclkLstIcons() 
{
  if (UpdateData(TRUE)) {
    EndDialog(IDOK);
  }
}

void CChangeIconDlg::OnButIcobrowse() 
{
  CWaitCursor wait;

  CString strFilter;
  strFilter.LoadString(IDS_TXT_FILTER2);
  strFilter += _T('|');

  UpdateData(TRUE);     // save all changes that occured in the GUI

  CFileDialog dlgFile(TRUE, NULL, m_edtIcofile_val, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, strFilter, this);

  if (dlgFile.DoModal() == IDOK) {
    m_edtIcofile_val = dlgFile.GetPathName();
    UpdateData(FALSE);  // update the GUI to reflect any changed settings
    UpdateIconList();
  }
}

void CChangeIconDlg::OnOK() 
{
  CWaitCursor wait;

  if (UpdateIconList()) {
  	CDialog::OnOK();
  }
}
