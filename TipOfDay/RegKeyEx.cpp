// RegKeyEx.cpp: implementation of the CRegKeyEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TipOfDay.h"
#include "RegKeyEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegKeyEx::CRegKeyEx() : CRegKey()
{

}

CRegKeyEx::~CRegKeyEx()
{

}

LONG CRegKeyEx::CreateDeep(HKEY hKeyParent, LPCTSTR lpszKeyName, LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition) {
  CString keyName(lpszKeyName);
  keyName.TrimLeft(_T('/'));
  keyName.TrimRight(_T('/'));

  if (lpdwDisposition != NULL)
    *lpdwDisposition = REG_OPENED_EXISTING_KEY;

  return CreateDeepHelper(hKeyParent, keyName, lpszClass, dwOptions, samDesired, lpSecAttr, lpdwDisposition);
}

LONG CRegKeyEx::CreateDeepHelper(HKEY hKeyParent, const CString& keyName, LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition) {
  if (keyName.IsEmpty()) {
    Attach(hKeyParent);
    return ERROR_SUCCESS;
  }

  CString keyNameLHS, keyNameRHS;

  keyNameLHS = keyName.SpanExcluding(_T("/"));
  keyNameRHS = keyName.Right(max(0, keyName.GetLength() - keyNameLHS.GetLength() - 1));

  LONG dwError;
  HKEY hKeyThis;
  DWORD dwDisposition_tmp;

  dwError = RegCreateKeyEx(hKeyParent, (LPCTSTR)keyNameLHS, 0, lpszClass, dwOptions, samDesired, lpSecAttr, &hKeyThis, &dwDisposition_tmp);

  if ((dwError != ERROR_SUCCESS) || (hKeyThis == NULL)) {
    return dwError;
  }

  if ((dwDisposition_tmp == REG_CREATED_NEW_KEY) && (lpdwDisposition != NULL)) {
    *lpdwDisposition = REG_CREATED_NEW_KEY;
  }

  dwError = CreateDeepHelper(hKeyThis, keyNameRHS, lpszClass, dwOptions, samDesired, lpSecAttr, lpdwDisposition);

  if ((HKEY)(*this) != hKeyThis)
    RegCloseKey(hKeyThis);

  return dwError;
}
