// RegKeyEx.h: interface for the CRegKeyEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGKEYEX_H__D10C7415_87A2_11D5_9D76_00A024112F81__INCLUDED_)
#define AFX_REGKEYEX_H__D10C7415_87A2_11D5_9D76_00A024112F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegKeyEx : public CRegKey
{
public:
	CRegKeyEx();
	virtual ~CRegKeyEx();

public:
  LONG CreateDeep(HKEY hKeyParent, LPCTSTR lpszKeyName, LPTSTR lpszClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE, REGSAM samDesired = KEY_ALL_ACCESS, LPSECURITY_ATTRIBUTES lpSecAttr = NULL, LPDWORD lpdwDisposition = NULL);

protected:
  LONG CreateDeepHelper(HKEY hKeyParent, const CString& keyName, LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition);
};

#endif // !defined(AFX_REGKEYEX_H__D10C7415_87A2_11D5_9D76_00A024112F81__INCLUDED_)
