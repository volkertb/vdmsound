// BasicBrowseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BasicBrowseDlg.h"

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
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBasicBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasicBrowseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasicBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CBasicBrowseDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg message handlers
