#if !defined(AFX_ICONLISTBOX_H__64713E36_5BB8_4B7A_BBD9_B1A32EA1E532__INCLUDED_)
#define AFX_ICONLISTBOX_H__64713E36_5BB8_4B7A_BBD9_B1A32EA1E532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CIconListBox window

class CIconListBox : public CListBox
{
// Construction
public:
	CIconListBox(int icoPadW = 2, int icoPadH = 2);

// Methods
public:
  int LoadIcons(LPCTSTR fName);
  void UnloadIcons(void);

// Member variables
protected:
  int m_icoPadW, m_icoPadH;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIconListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconListBox)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONLISTBOX_H__64713E36_5BB8_4B7A_BBD9_B1A32EA1E532__INCLUDED_)
