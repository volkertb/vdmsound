// TipOfDayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TipOfDay.h"
#include "TipOfDayDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TipOfDayDlg dialog


CTipOfDayDlg::CTipOfDayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTipOfDayDlg::IDD, pParent), m_pFileReader(NULL)
{
	//{{AFX_DATA_INIT(CTipOfDayDlg)
	m_ShowStartupVal = FALSE;
	m_TipIDVal = _T("");
	//}}AFX_DATA_INIT
}


void CTipOfDayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipOfDayDlg)
	DDX_Control(pDX, IDC_S_TIPID, m_TipIDCtl);
	DDX_Control(pDX, IDC_B_NEXTTIP, m_NextTipCtl);
	DDX_Control(pDX, IDC_B_CLOSE, m_CloseCtl);
	DDX_Control(pDX, IDC_E_TIPTEXT, m_TipTextCtl);
	DDX_Control(pDX, IDC_S_TIPTITLE, m_TipTitleCtl);
	DDX_Check(pDX, IDC_C_SHOWSTARTUP, m_ShowStartupVal);
	DDX_Text(pDX, IDC_S_TIPID, m_TipIDVal);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// TipOfDayDlg utility functions

void CTipOfDayDlg::SetTipText(const CString& text, int tipID, int numTips) {
  CString tipIDTxt;
  tipIDTxt.Format(_T("%d / %d"), tipID + 1, numTips);

  m_TipTextCtl.SetWindowText(text);
  m_TipIDCtl.SetWindowText(numTips > 0 ? tipIDTxt : _T(""));

  SCROLLINFO scrollInfo;
  m_TipTextCtl.ShowScrollBar(SB_VERT, TRUE);

  if (m_TipTextCtl.GetScrollInfo(SB_VERT, &scrollInfo) && (scrollInfo.nMin + scrollInfo.nMax < (int)scrollInfo.nPage)) {
    m_TipTextCtl.ShowScrollBar(SB_VERT, FALSE);
  }
}


BEGIN_MESSAGE_MAP(CTipOfDayDlg, CDialog)
	//{{AFX_MSG_MAP(CTipOfDayDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_B_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_B_NEXTTIP, OnNextTip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TipOfDayDlg message handlers

BOOL CTipOfDayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

  /* set dialog icon and caption */
  SetIcon(::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_APPICON)), TRUE);
  SetWindowText(m_pFileReader != NULL ? m_pFileReader->GetTitle() : _T(""));

  /* change title font */
  LOGFONT logicalFont;

  m_TipTitleCtl.GetFont()->GetLogFont(&logicalFont);
  logicalFont.lfHeight  = (LONG)(1.5 * logicalFont.lfHeight);
  logicalFont.lfWeight  = FW_BOLD;
  logicalFont.lfQuality = PROOF_QUALITY;
  m_TipTitleFont.CreateFontIndirect(&logicalFont);
  m_TipTitleCtl.SetFont(&m_TipTitleFont);

  /** add text **/
  if ((m_pFileReader != NULL) && (m_pFileReader->GetNumTips() > 0)) {
    m_tipID = max(0, min(m_pFileReader->GetNumTips() - 1, m_tipID));

    m_TipTextCtl.EnableWindow(TRUE);
    m_NextTipCtl.EnableWindow(TRUE);
    SetTipText(m_pFileReader->GetTip(m_tipID), m_tipID, m_pFileReader->GetNumTips());
  } else {
    CString errMsg;
    errMsg.FormatMessage(IDS_ERR_1);

    m_TipTextCtl.EnableWindow(FALSE);
    m_NextTipCtl.EnableWindow(FALSE);
    SetTipText(errMsg, 0, 0);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTipOfDayDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  switch (nCtlColor) {
    case CTLCOLOR_BTN:
    case CTLCOLOR_DLG:
    case CTLCOLOR_EDIT:
    case CTLCOLOR_LISTBOX:
    case CTLCOLOR_STATIC:
      pDC->SetTextColor(GetSysColor(COLOR_INFOTEXT));
      pDC->SetBkColor(GetSysColor(COLOR_INFOBK));
      return ::GetSysColorBrush(COLOR_INFOBK);

    default:
      return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
  }
}

void CTipOfDayDlg::OnClose() 
{
	// TODO: Add your control notification handler code here
  UpdateData();
	EndDialog(IDOK);
}

void CTipOfDayDlg::OnNextTip() 
{
	// TODO: Add your control notification handler code here
  if ((m_pFileReader != NULL) && (m_pFileReader->GetNumTips() > 0)) {
    m_tipID = (m_tipID + 1) % m_pFileReader->GetNumTips();

    m_TipTextCtl.EnableWindow(TRUE);
    m_NextTipCtl.EnableWindow(TRUE);
    SetTipText(m_pFileReader->GetTip(m_tipID), m_tipID, m_pFileReader->GetNumTips());
  } else {
    CString errMsg;
    errMsg.FormatMessage(IDS_ERR_1);

    m_TipTextCtl.EnableWindow(FALSE);
    m_NextTipCtl.EnableWindow(FALSE);
    SetTipText(errMsg, 0, 0);
  }
}
