#include "stdafx.h"
#include "MFCUtil.h"

/////////////////////////////////////////////////////////////////////////////
//
// Message-formatting functions
//
/////////////////////////////////////////////////////////////////////////////

//
// Formats a CString message.
//
HRESULT __FormatMessage(
    CString& Buffer,        // buffer where string will be stored
    BYTE nMaxSize,          // maximum size of a line after formatting; 0 for no restrictions
    LPCTSTR lpFormat,       // literal message definition; if NULL, dwMessageID is used as a numeric message definition
    DWORD dwMessageID,      // index into message-table resource where the message definition can be found
    DWORD dwLanguageID,     // language to be used; meaningless when the message definition was provided in lpFormat
    HMODULE hModule,        // module containing the message-table resources; meaningless when the message definition was provided in lpFormat; NULL means current process' application image
    bool bFromSystem,       // use the system message-table resources instead of application-provided ones to resolve GetLastError() codes
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    va_list* Arguments )    // arguments to be used when replacing inserts
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

  DWORD   dwRetVal = 0;
  LPCTSTR lpMsgBuf = NULL;
  DWORD   dwFlags  = FORMAT_MESSAGE_ALLOCATE_BUFFER | (nMaxSize & FORMAT_MESSAGE_MAX_WIDTH_MASK);
  LPCVOID lpSource = NULL;

  if (lpFormat == NULL) {
    if (bFromSystem) {
      dwFlags |= FORMAT_MESSAGE_FROM_SYSTEM;
    } else {
      dwFlags |= FORMAT_MESSAGE_FROM_HMODULE;
      if (hModule == NULL) {
        lpSource = (LPCVOID)AfxGetInstanceHandle();
      } else {
        lpSource = (LPCVOID)hModule;
      }
    }
  } else {
    dwFlags |= FORMAT_MESSAGE_FROM_STRING;
    lpSource = (LPCVOID)lpFormat;
  }

  if (bIgnoreInserts) {
    dwFlags |= FORMAT_MESSAGE_IGNORE_INSERTS;
  }

  dwRetVal = FormatMessage(dwFlags, lpSource, dwMessageID, dwLanguageID, (LPTSTR)&lpMsgBuf, 0, Arguments);

  if (lpMsgBuf != NULL) {
    Buffer = lpMsgBuf;
    LocalFree((LPVOID)lpMsgBuf);
  }

  if (dwRetVal == 0) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  if (bFromSystem) {
    Buffer.TrimRight(_T("\n\r\t "));
  }

  return S_OK;
}

//
// Formats a CString message based on a provided format string
//
HRESULT AFXAPI FormatMessageV(
    CString& Buffer,        // buffer where string will be stored
    LPCTSTR lpFormat,       // literal message definition
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    va_list* Arguments )
{
  if (lpFormat == NULL) {
    return E_POINTER;
  } else {
    return __FormatMessage(Buffer, 0, lpFormat, 0, 0, 0, false, bIgnoreInserts, Arguments);
  }
}

HRESULT AFX_CDECL FormatMessage(
    CString& Buffer,        // buffer where string will be stored
    LPCTSTR lpFormat,       // literal message definition
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    ... )
{
  DWORD dwRetVal;
  va_list Arguments;
  va_start(Arguments, bIgnoreInserts);
  dwRetVal = FormatMessageV(Buffer, lpFormat, bIgnoreInserts, &Arguments);
  va_end(Arguments);
  return dwRetVal;
}

CString AFX_CDECL FormatMessage(
    LPCTSTR lpFormat,       // literal message definition
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    ... )
{
  CString Buffer;
  va_list Arguments;
  va_start(Arguments, bIgnoreInserts);
  FormatMessageV(Buffer, lpFormat, bIgnoreInserts, &Arguments);
  va_end(Arguments);
  return Buffer;
}

//
// Formats a CString message based on a string-table resource ID
//
HRESULT AFXAPI FormatMessageV(
    CString& Buffer,        // buffer where string will be stored
    DWORD dwMessageID,      // index into message-table resource where the message definition can be found
    bool bFromSystem,       // use the system message-table resources instead of application-provided ones to resolve GetLastError() codes
    HMODULE hModule,        // module containing the message-table resources; NULL means current process' application image
    DWORD dwLanguageID,     // language to be used; use 0 or MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) for default
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    va_list* Arguments )
{
  return __FormatMessage(Buffer, 0, NULL, dwMessageID, dwLanguageID, hModule, bFromSystem, bIgnoreInserts, Arguments);
}

HRESULT AFX_CDECL FormatMessage(
    CString& Buffer,        // buffer where string will be stored
    DWORD dwMessageID,      // index into message-table resource where the message definition can be found
    bool bFromSystem,       // use the system message-table resources instead of application-provided ones to resolve GetLastError() codes
    HMODULE hModule,        // module containing the message-table resources; NULL means current process' application image
    DWORD dwLanguageID,     // language to be used; use 0 or MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) for default
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    ... )
{
  DWORD dwRetVal = 0;
  va_list Arguments;
  va_start(Arguments, bIgnoreInserts);
  dwRetVal = FormatMessageV(Buffer, dwMessageID, bFromSystem, hModule, dwLanguageID, bIgnoreInserts, &Arguments);
  va_end(Arguments);
  return dwRetVal;
}

CString AFX_CDECL FormatMessage(
    DWORD dwMessageID,      // index into message-table resource where the message definition can be found
    bool bFromSystem,       // use the system message-table resources instead of application-provided ones to resolve GetLastError() codes
    HMODULE hModule,        // module containing the message-table resources; NULL means current process' application image
    DWORD dwLanguageID,     // language to be used; use 0 or MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) for default
    bool bIgnoreInserts,    // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
    ... )
{
  CString Buffer;
  va_list Arguments;
  va_start(Arguments, bIgnoreInserts);
  FormatMessageV(Buffer, dwMessageID, bFromSystem, hModule, dwLanguageID, bIgnoreInserts, &Arguments);
  va_end(Arguments);
  return Buffer;
}

//
//
//
CString AFX_CDECL Format(
    LPCTSTR lpFormat,         // A format-control string
    ... )
{
  CString Buffer;
  va_list Arguments;
  va_start(Arguments, lpFormat);
  Buffer.FormatV(lpFormat, Arguments);
  va_end(Arguments);
  return Buffer;
}

//
// Eliminates any characters belonging to a given set (toClean) from the
//   vicinity of a given position (nPos) in a given string (str).
//
int __strClean(CString& str, const CString& toClean, int nPos) {
  int nStart, nEnd;

  for (nStart = nPos; (nStart > -1) && (toClean.Find(str.GetAt(nStart)) != -1); nStart--);
  for (nEnd = nPos; (nEnd < str.GetLength()) && (toClean.Find(str.GetAt(nEnd)) != -1); nEnd++);

  nStart++;

  if ((nEnd - nStart) > 0)
    str.Delete(nStart, nEnd - nStart);

  return nStart;
}

//
// Reformats a string in order to make it fit within a given
//   number of columns by wrapping the text.
//
void AFXAPI WrapString(
    CString& Str,             // string to wrap
    int nWrap,                // how many columns to wrap into
    LPCTSTR Indent1,          // string to indent the first line of a paragraph with
    LPCTSTR Indent2,          // string to indent every other line of a paragraph with
    LPCTSTR Breakable)        // what characters can serve as break points
{
  ASSERT(nWrap > 0);

  // Translate LPCSTRs into CStrings for extra functionality
  CString Separators = Breakable;
  CString flIndent   = Indent1;
  CString olIndent   = Indent2;

  // Control positions in string
  int curPos, lastSepPos = -1, lastLFPos = -1;

  // Indent first line
  Str.Insert(0, flIndent);

  // Scan string until its end is reached
  for (curPos = 0; curPos < Str.GetLength(); curPos++) {
    TCHAR curChr = Str.GetAt(curPos);

    // A hard break (start of paragraph)?
    if (curChr == _T('\n')) {
      lastLFPos = curPos;                 // last newline was here
      lastSepPos = -1;                    // no separators found on this line (yet)
      Str.Insert(curPos + 1, flIndent);   // first-line indent this paragraph
      continue;
    }

    // Did we hit a separator?
    if (Separators.Find(curChr) != -1) {
      lastSepPos = curPos;                // this is a candidate for a soft linebreak (for wrapping)
    }

    // Are we past the wrap column?
    if ((curPos - lastLFPos) > nWrap) {
      // Any candidates for soft linebreaks found yet?
      if (lastSepPos != -1) {
        curPos = __strClean(Str, Separators, lastSepPos); // erase all separators around soft linebreak
        Str.Insert(curPos, _T("\n") + olIndent);          // insert soft linebreak + indentation
        lastLFPos = curPos;               // new start of line
        lastSepPos = -1;                  // no separators found on this line (yet)
      }
      continue;
    }
  }
}

//
// Reformats a string in order to make it fit within a given
//   number of columns by wrapping the text.
//
CString AFXAPI LoadString(
    UINT nID)                 // A Windows string resource ID
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString retVal;

  try {
    retVal.LoadString(nID);
  } catch (CMemoryException& /*cme*/) { }

  return retVal;
}



/////////////////////////////////////////////////////////////////////////////
//
// Message-box functions
//
/////////////////////////////////////////////////////////////////////////////

struct __HLPINFO {
  LPCTSTR lpszHelp;
  DWORD   dwContextHelpId;
};

//
// WM_HELP call back procedure
//
VOID CALLBACK __MsgBoxCallback(LPHELPINFO lpHelpInfo) {
  if (lpHelpInfo == NULL)
    return;

  __HLPINFO* lpInfo = (__HLPINFO*)(lpHelpInfo->dwContextId);

  WinHelp(NULL, lpInfo->lpszHelp, HELP_CONTEXT, lpInfo->dwContextHelpId);
}

//
//
//
int AFXAPI MessageBox(
    LPCTSTR lpszText,       // address of text in message box
    LPCTSTR lpszCaption,    // address of title of message box
    UINT uButtonType,       // buttons contained in the message box
    UINT uIconType,         // display an icon in the message box
    LPCTSTR lpszHelp,       // the path, if necessary, and the name of the Help file that WinHelp is to display
    DWORD dwContextHelpId,  // context identifier for the help topic
    UINT uModal,            // the modality of the dialog box
    bool bSetFg)            // the message box becomes the foreground window
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

  __HLPINFO hlpInfo;
  MSGBOXPARAMS params;

  hlpInfo.lpszHelp          = lpszHelp;
  hlpInfo.dwContextHelpId   = dwContextHelpId;

  params.cbSize             = sizeof(params);
  params.hwndOwner          = NULL;
  params.hInstance          = NULL;
  params.lpszText           = lpszText;
  params.lpszCaption        = lpszCaption;
  params.dwStyle            = uButtonType|uIconType|uModal;
  params.lpszIcon           = NULL;
  params.dwContextHelpId    = (DWORD)(&hlpInfo);
  params.lpfnMsgBoxCallback = __MsgBoxCallback;
  params.dwLanguageId       = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

  if (bSetFg)
    params.dwStyle |= MB_SETFOREGROUND;

  if (lpszHelp != NULL)
    params.dwStyle |= MB_HELP;

  return MessageBoxIndirect(&params);
}

//
//
//
LPCTSTR AFXAPI GetHelpPath(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CWinApp* lpApp = AfxGetApp();

  if (lpApp == NULL) {
    return NULL;
  } else {
    return lpApp->m_pszHelpFilePath;
  }
}

//
//
//
LPCSTR AFXAPI SearchPathA(
  LPCSTR szFileName )
{
  static CHAR buf[_MAX_PATH];
  LPSTR lpFilePart;

  if (SearchPathA(NULL, szFileName, NULL, _MAX_PATH, buf, &lpFilePart) == 0) {
    return szFileName;
  } else {
    return buf;
  }
}
