#if !defined(AFX_TIPOFDAYDLG_H__4F5EA18A_8468_11D5_9D73_00A024112F81__INCLUDED_)
#define AFX_TIPOFDAYDLG_H__4F5EA18A_8468_11D5_9D73_00A024112F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipOfDayDlg.h : header file
//

class CTipFileReader;

/////////////////////////////////////////////////////////////////////////////
// TipOfDayDlg dialog

class CTipOfDayDlg : public CDialog
{
// Construction
public:
	CTipOfDayDlg(CWnd* pParent = NULL);  // standard constructor

// Accessors
public:
  BOOL GetShowStartup(void) { return m_ShowStartupVal; }
  DWORD GetTipID(void) { return m_tipID; }
  void SetShowStartup(BOOL showStartup) { m_ShowStartupVal = showStartup; }
  void SetTipID(DWORD tipID) { m_tipID = tipID; }

  void SetTipFile(CTipFileReader& fileReader) { m_pFileReader = &fileReader; };

protected:
  void SetTipText(const CString& text, int tipID, int numTips);

protected:
  CFont m_TipTitleFont;
  CBrush m_bgBrush;

  CTipFileReader* m_pFileReader;

	long	m_tipID;

// Dialog Data
	//{{AFX_DATA(CTipOfDayDlg)
	enum { IDD = IDD_TIPOFDAYDLG_DIALOG };
	CStatic	m_TipIDCtl;
	CButton	m_NextTipCtl;
	CButton	m_CloseCtl;
	CEdit	m_TipTextCtl;
	CStatic	m_TipTitleCtl;
	BOOL	m_ShowStartupVal;
	CString	m_TipIDVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipOfDayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTipOfDayDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnNextTip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPOFDAYDLG_H__4F5EA18A_8468_11D5_9D73_00A024112F81__INCLUDED_)
