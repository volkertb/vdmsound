#if !defined(AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_)
#define AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeIconDlg.h : header file
//

#include "IconListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg dialog

class CChangeIconDlg : public CDialog
{
// Construction
public:
	CChangeIconDlg(CWnd* pParent = NULL);   // standard constructor

protected:
  void UpdateIconList(void);

public:
// Dialog Data
	//{{AFX_DATA(CChangeIconDlg)
	enum { IDD = IDD_CHANGEICON };
	CIconListBox m_lstIcons;
	CString	m_edtFile_val;
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
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEdtFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEICONDLG_H__BC8E588B_3A94_41A8_A331_CA6B0DCF91B5__INCLUDED_)
