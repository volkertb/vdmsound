#if !defined(AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_)
#define AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicBrowseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CBasicBrowseDlg dialog

class CBasicBrowseDlg : public CDialog
{
// Construction
public:
	CBasicBrowseDlg(CWnd* pParent = NULL);   // standard constructor

// Member variables
protected:
  VLPUtil::CComKeepAlive _autocomplete;    // COM must be present during the entire lifespan of any autocomplete controls
  CContextHelp m_help;

// Public MFC stuff
public:
  CString m_iconLocation;   // icon location (file name), not shown in CBasicBrowseDlg

// Dialog Data
	//{{AFX_DATA(CBasicBrowseDlg)
	enum { IDD = IDD_BASICBROWSE };
	CEdit	m_edtDosdir;
	CEdit	m_edtDosprogram;
	CString	m_edtDosargs_val;
	CString	m_edtDosprogram_val;
	CString	m_edtDosdir_val;
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
	virtual BOOL OnInitDialog();
	afx_msg void OnButDosprogrambrowse();
	afx_msg void OnButChangeicon();
	afx_msg void OnUpdateEdtdosprogram();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICBROWSEDLG_H__DCD63CA6_7C89_485D_A05C_8AB1BA248E7F__INCLUDED_)
