// TipOfDay.h : main header file for the TIPOFDAY DLL
//

#if !defined(AFX_TIPOFDAY_H__4F5EA183_8468_11D5_9D73_00A024112F81__INCLUDED_)
#define AFX_TIPOFDAY_H__4F5EA183_8468_11D5_9D73_00A024112F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "RegKeyEx.h"
#include "TipFileReader.h"

/////////////////////////////////////////////////////////////////////////////
// CTipOfDayApp
// See TipOfDay.cpp for the implementation of this class
//

class CTipOfDayApp : public CWinApp
{
public:
	CTipOfDayApp();

public:
  DWORD Initialize(const CString& keyName);
  DWORD IsEnabled(DWORD& isEnabled);
  DWORD ShowTips(void);

protected:
  DWORD CfgGet_ShowTips(DWORD& dwValue);
  DWORD CfgGet_NextTip(DWORD& dwValue);

  DWORD CfgSet_ShowTips(DWORD dwValue);
  DWORD CfgSet_NextTip(DWORD dwValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipOfDayApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTipOfDayApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  CRegKeyEx m_cfgKey;
  CTipFileReader m_tipFile;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPOFDAY_H__4F5EA183_8468_11D5_9D73_00A024112F81__INCLUDED_)
