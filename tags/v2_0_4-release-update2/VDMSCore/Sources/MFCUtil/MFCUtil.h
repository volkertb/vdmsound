#ifndef __MFCUTIL_H_
#define __MFCUTIL_H_

HRESULT AFXAPI FormatMessageV(CString& Buffer, LPCTSTR lpFormat, bool bIgnoreInserts, va_list* Arguments);
HRESULT AFXAPI FormatMessageV(CString& Buffer, DWORD dwMessageID, bool bFromSystem, HMODULE hModule, DWORD dwLanguageID, bool bIgnoreInserts, va_list* Arguments);

HRESULT AFX_CDECL FormatMessage(CString& Buffer, LPCTSTR lpFormat, bool bIgnoreInserts = true, ...);
HRESULT AFX_CDECL FormatMessage(CString& Buffer, DWORD dwMessageID, bool bFromSystem = true, HMODULE hModule = NULL, DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), bool bIgnoreInserts = true, ...);

CString AFX_CDECL FormatMessage(LPCTSTR lpFormat, bool bIgnoreInserts = true, ...);
CString AFX_CDECL FormatMessage(DWORD dwMessageID, bool bFromSystem = true, HMODULE hModule = NULL, DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), bool bIgnoreInserts = true, ...);

CString AFX_CDECL Format(LPCTSTR lpFormat, ...);

void AFXAPI WrapString(CString& Str, int nWrap, LPCTSTR Indent1 = NULL, LPCTSTR IndentX = NULL, LPCTSTR Breakable = _T(" "));

CString AFXAPI LoadString(UINT nID);

int AFXAPI MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uButtonType, UINT uIconType = 0, LPCTSTR lpszHelp = NULL, DWORD dwContextHelpId = 0, UINT uModal = MB_TASKMODAL, bool bSetFg = true);

LPCTSTR AFXAPI GetHelpPath(void);

LPCSTR AFXAPI SearchPathA(LPCSTR fileName);

CString GetExceptionText(DWORD dwStatus);

#endif //__MFCUTIL_H_
