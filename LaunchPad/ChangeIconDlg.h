#if !defined(AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_)
#define AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeIconDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg dialog

class CChangeIconDlg : public CDialog
{
// Construction
public:
	CChangeIconDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeIconDlg)
	enum { IDD = IDD_CHANGEICON };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeIconDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeIconDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_)
