#if !defined(AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_)
#define AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicBrowseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg dialog

class CBasicBrowseDlg : public CDialog
{
// Construction
public:
	CBasicBrowseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBasicBrowseDlg)
	enum { IDD = IDD_BASICBROWSE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicBrowseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBasicBrowseDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_)
