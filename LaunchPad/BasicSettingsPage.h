#if !defined(AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_)
#define AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicSettingsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicSettingsPage dialog

class CBasicSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBasicSettingsPage)

// Construction
private:
	CBasicSettingsPage();
	~CBasicSettingsPage();

public:
  static HRESULT AddPage(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam, const CStringArray& fileNames);

// Dialog Data
	//{{AFX_DATA(CBasicSettingsPage)
	enum { IDD = IDD_PROPPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBasicSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBasicSettingsPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member variables
protected:
  CStringArray m_fileNames;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICSETTINGSPAGE_H__A54A813E_F49A_451F_97A9_EB117FF36091__INCLUDED_)
