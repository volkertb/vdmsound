#include "stdafx.h"
#include "resource.h"
#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

#include <delayimp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////

#define IDI_SH_UNKNOWN      1
#define IDI_SH_MULTISEL   133

//////////////////////////////////////////////////////////////////////
//
// CStatic_Icon
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatic_Icon::CStatic_Icon(void) : CStatic(), m_hIcon(NULL), m_bAttached(FALSE) {
}

CStatic_Icon::~CStatic_Icon(void) {
  if (m_bAttached)
    DeleteIcon();

  CStatic::~CStatic();
}



//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

HRESULT CStatic_Icon::LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName) {
  HICON hIcon;

  if (m_bAttached)
    DeleteIcon();

  if ((hIcon = ::LoadIcon(hInstance, lpIconName)) == NULL)
    return HRESULT_FROM_WIN32(GetLastError());

  m_hIcon = hIcon;
  m_bAttached = FALSE;    // do not attach because the icon instance is shared and should not be destroyed

  SetIcon(m_hIcon);

  return S_OK;
}

HRESULT CStatic_Icon::ExtractIcon(HINSTANCE hInstance, LPCTSTR lpszExeFileName, UINT nIconIndex) {
  HICON hIcon;

  if (m_bAttached)
    DeleteIcon();

  if ((hIcon = ::ExtractIcon(hInstance, lpszExeFileName, nIconIndex)) < (HICON)2)
    return E_FAIL;

  m_hIcon = hIcon;
  m_bAttached = TRUE;     // attach because the icon instance must be destroyed when we are done with it

  SetIcon(m_hIcon);

  return S_OK;
}

BOOL CStatic_Icon::DeleteIcon(void) {
	if (m_hIcon == NULL)
		return FALSE;

  SetIcon(NULL);          // make sure the icon is not selected in any device context

  BOOL retVal = ::DestroyIcon(m_hIcon);

  m_hIcon = NULL;
  m_bAttached = FALSE;

  return retVal;
}



//////////////////////////////////////////////////////////////////////
//
// CDIBitmap
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDIBitmap::CDIBitmap(void) : CBitmap() {
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

BOOL CDIBitmap::CreateDIBitmap(CDC* pdc, const LPBITMAPINFOHEADER lpbmih, LPCVOID lpbInit, const LPBITMAPINFO lpbmi, UINT fuUsage) {
  ASSERT(lpbmih != NULL);

  if (lpbmih == NULL)
    return FALSE;

  return Attach(::CreateDIBitmap(pdc->GetSafeHdc(), lpbmih, lpbInit != NULL, lpbInit, lpbmi, fuUsage));
}

BOOL CDIBitmap::CreateDIBitmap(int nWidth, int nHeight, UINT nBitcount, int numColors) {
  ASSERT(!((numColors < 0) || (numColors > 256)));

  BITMAPINFOHEADER bmih;

  bmih.biSize          = sizeof(BITMAPINFOHEADER);
  bmih.biWidth         = nWidth;
  bmih.biHeight        = nHeight;
  bmih.biPlanes        = 1;
  bmih.biBitCount      = nBitcount;
  bmih.biCompression   = nBitcount >= 16 ? BI_RGB : BI_BITFIELDS;
  bmih.biSizeImage     = 0;
  bmih.biXPelsPerMeter = 0;
  bmih.biYPelsPerMeter = 0;
  bmih.biClrUsed       = numColors;
  bmih.biClrImportant  = numColors;

  CDC tmpDC;
  tmpDC.Attach(GetDC(NULL));

  return CreateDIBitmap(&tmpDC, &bmih, NULL, NULL);
}

UINT CDIBitmap::SetDIBColorTable(CDC* pdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors) {
  return ::SetDIBColorTable(pdc->GetSafeHdc(), uStartIndex, cEntries, pColors);
}

UINT CDIBitmap::SetDIBits(CDC* pdc, UINT uStartScan, UINT cScanLines, CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi, UINT fuColorUse) {
  return ::SetDIBits(pdc->GetSafeHdc(), (HBITMAP)GetSafeHandle(), uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);
}



//////////////////////////////////////////////////////////////////////
//
// COpenDOSProgramDialog
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenDOSProgramDialog::COpenDOSProgramDialog(
  LPCTSTR lpszFileName,
  CWnd* pParentWnd)
 : CFileDialog(TRUE, NULL, lpszFileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, VLPUtil::LoadString(IDS_TXT_FILTER1), pParentWnd)
{
}

//////////////////////////////////////////////////////////////////////
// Overridables
//////////////////////////////////////////////////////////////////////

BOOL COpenDOSProgramDialog::OnFileNameOK(void) {
  // Check that the file is a MS-DOS .exe, .com, or .bat file
  if (VLPUtil::isMSDOSFile(GetPathName())) {  // valid .exe/.com file
    return FALSE;
  } else {
    CString strTmp;
    strTmp.FormatMessage(IDS_MSG_NOEXECOMBATERR, (LPCTSTR)GetFileName());
    return (MessageBox(strTmp, NULL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES);
  }
}



//////////////////////////////////////////////////////////////////////
//
// VLPUtil
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Delayed imports
//////////////////////////////////////////////////////////////////////

#ifndef SHACF_DEFAULT
// Force declaration even for older versions of IE; the delayed imports binding
//  wil take care of any problems with missing exports or dll's etc. at runtime.

#define SHACF_DEFAULT                   0x00000000  // Currently (SHACF_FILESYSTEM | SHACF_URLALL)
#define SHACF_FILESYSTEM                0x00000001  // This includes the File System as well as the rest of the shell (Desktop\My Computer\Control Panel\)
#define SHACF_URLALL                    (SHACF_URLHISTORY | SHACF_URLMRU)
#define SHACF_URLHISTORY                0x00000002  // URLs in the User's History
#define SHACF_URLMRU                    0x00000004  // URLs in the User's Recently Used list.
#define SHACF_USETAB                    0x00000008  // Use the tab to move thru the autocomplete possibilities instead of to the next dialog/window control.

#define SHACF_AUTOSUGGEST_FORCE_ON      0x10000000  // Ignore the registry default and force the feature on.
#define SHACF_AUTOSUGGEST_FORCE_OFF     0x20000000  // Ignore the registry default and force the feature off.
#define SHACF_AUTOAPPEND_FORCE_ON       0x40000000  // Ignore the registry default and force the feature on. (Also know as AutoComplete)
#define SHACF_AUTOAPPEND_FORCE_OFF      0x80000000  // Ignore the registry default and force the feature off. (Also know as AutoComplete)

LWSTDAPI SHAutoComplete(HWND hwndEdit, DWORD dwFlags);

#endif

//////////////////////////////////////////////////////////////////////
// Private utility function(s)
//////////////////////////////////////////////////////////////////////

//
//
//
BOOL StrSplit(LPCTSTR szSrc, TCHAR sep, CString& lhs, CString& rhs, BOOL fromBeginning = TRUE, BOOL wantRHS = TRUE) {
  int splitPos;
  CString strSrc(szSrc);
  BOOL retVal = TRUE;

  if (fromBeginning) {
    if ((splitPos = strSrc.Find(sep)) < 0) {
      splitPos = wantRHS ? -1 : strSrc.GetLength();
      retVal = FALSE;
    }
  } else {
    if ((splitPos = strSrc.ReverseFind(sep)) < 0) {
      splitPos = wantRHS ? -1 : strSrc.GetLength();
      retVal = FALSE;
    }
  }

  lhs = strSrc.Left(max(0, splitPos));
  rhs = strSrc.Mid(min(strSrc.GetLength() - 1, splitPos + 1), max(0, strSrc.GetLength() - (splitPos + 1)));

  return retVal;
}

LPCTSTR szSeparators = _T("\n\r\t ");

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
    Buffer.TrimRight(szSeparators);
  }

  return S_OK;
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

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////

LPCTSTR VLPUtil::T_YES = _T("yes");
LPCTSTR VLPUtil::T_NO = _T("no");

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
// Synchronizes the state of a Win32 check-box with its corresponding
//  setting
//
HRESULT VLPUtil::SyncCheckBox(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  BOOL defState,                                      // default button state assumed if none is defined
  LPCTSTR yesValue,                                   // ini value corresponding to a checked state
  LPCTSTR noValue)                                    // ini value corresponding to an unchecked state
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);
  ASSERT(yesValue != NULL);
  ASSERT(noValue != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (bSave) {
    switch (control.GetState() & 0x3) {
      case BST_INDETERMINATE:
        return settings.UnsetValue(section, key);
      case BST_CHECKED:
        return settings.SetValue(section, key, yesValue);
      case BST_UNCHECKED:
        return settings.SetValue(section, key, noValue);
      default:
        return E_UNEXPECTED;
    }
  } else {
    HRESULT hr = S_OK;
    LPCTSTR defValue = defState ? yesValue : noValue; // default value based on default state
    BOOL isIndeterminate = FALSE;
    CString value = defValue;

    hr = settings.GetValue(section, key, value, &isIndeterminate, defValue);
    value.TrimLeft(); value.TrimRight();

    control.EnableWindow(SUCCEEDED(hr));              // disable the control if an error occured

    int bState;

    if (isIndeterminate || FAILED(hr)) {
      bState = BST_INDETERMINATE;
    } else if (value.CollateNoCase(yesValue) == 0) {
      bState = BST_CHECKED;
    } else if (value.CollateNoCase(noValue) == 0) {
      bState = BST_UNCHECKED;
    } else {
      bState = BST_INDETERMINATE;
    }

    control.SetButtonStyle(bState == BST_INDETERMINATE ? BS_AUTO3STATE : BS_AUTOCHECKBOX);
    control.SetCheck(bState);

    return hr;
  }
}

//
// Synchronizes the state of a Win32 radio-button with its corresponding
//  setting.
//  This function deals with 'regular' radio buttons (that represent
//  meaningful settings).
//
HRESULT VLPUtil::SyncRadioButton(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  BOOL defState,                                      // default button state assumed if none is defined
  LPCTSTR selValue)                                   // ini value corresponding to a selected state
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);
  ASSERT(selValue != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (bSave) {
    if ((control.GetState() & 0x3) == BST_CHECKED) {
      return settings.SetValue(section, key, selValue);
    } else {
      return S_OK;
    }
  } else {
    HRESULT hr = S_OK;
    LPCTSTR defValue = defState ? selValue : _T("");  // default value based on default state
    BOOL isIndeterminate = FALSE;
    CString value = defValue;

    hr = settings.GetValue(section, key, value, &isIndeterminate, defValue);
    value.TrimLeft(); value.TrimRight();

    control.EnableWindow(SUCCEEDED(hr));              // disable the control if an error occured

    int bState;

    if (isIndeterminate || FAILED(hr)) {
      bState = BST_UNCHECKED;
    } else if (value.CollateNoCase(selValue) == 0) {
      bState = BST_CHECKED;
    } else {
      bState = BST_UNCHECKED;
    }

    control.SetButtonStyle(BS_AUTORADIOBUTTON);
    control.SetCheck(bState);

    return hr;
  }
}

//
// Synchronizes the state of a Win32 radio-button with its corresponding
//  setting.
//  This function deals with 'other'-type radio buttons that represent
//  indeterminate settings.
//
HRESULT VLPUtil::SyncRadioButton(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  CString& otherValueStorage,                         // caller-provided temporary storage for "other" data
  LPCTSTR selValue1,                                  // list of known values for which this button should not be in a selected state
  ...)                                                //  (list must end with a NULL value)
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);
  ASSERT(selValue1 != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (bSave) {
    if ((control.GetState() & 0x3) == BST_CHECKED) {  // "Other" option is selected
      if (otherValueStorage.IsEmpty())
        return E_FAIL;  // if "Other" ever got selected/enabled (via bSave==FALSE) then we should have *something* in otherValueStorage

      switch (otherValueStorage.GetAt(0)) {
        case _T('0'):   // "indeterminate"
          return settings.UnsetValue(section, key);   // reset
        case _T('1'):   // "determinate"
          return settings.SetValue(section, key, otherValueStorage.Mid(1));
        default:
          return E_FAIL;
      }
    } else {
      return S_OK;
    }
  } else {
    va_list argList;
    va_start(argList, selValue1);

    HRESULT hr = S_OK;
    CString value;
    BOOL isIndeterminate = FALSE;

    hr = settings.GetValue(section, key, value, &isIndeterminate, _T(""));
    value.TrimLeft(); value.TrimRight();

    BOOL isSelected = TRUE;
    BOOL isEnabled  = SUCCEEDED(hr);                  // disable the control if an error occured

    if (!isIndeterminate) {
      for (LPCTSTR arg = selValue1; arg != NULL; arg = va_arg(argList, LPCTSTR)) {
        if (value.CollateNoCase(arg) == 0) {
          isSelected = FALSE;                         // another radio matched, so do not select "Other"
          isEnabled &= !otherValueStorage.IsEmpty();  // If we had a "Other" value lately then keep the control enabled, even if not selected
          break;
        }
      }

      if (isSelected) {
        otherValueStorage = _T("1") + value;          // store "determinate" (non-empty) value if it doesn't correspind to any of the known (non-"Other") values, i.e. selValue1/2/3...
      }
    } else {
      otherValueStorage = _T("0");                    // remember as "indeterminate"
    }

    control.EnableWindow(isEnabled);
    control.SetButtonStyle(BS_AUTORADIOBUTTON);
    control.SetCheck(isSelected ? BST_CHECKED : BST_UNCHECKED);

    va_end(argList);

    return hr;
  }
}

//
// Synchronizes the state of a Win32 radio-button with its corresponding
//  setting.
//  This function deals with 'regular' radio buttons (that represent
//  meaningful settings).
//
HRESULT VLPUtil::SyncEditBox(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CWnd& control,                                      // window control with which the data must be synchronized
  LPCTSTR defValue)                                   // default value
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);
  ASSERT(defValue != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  HRESULT hr = S_OK;
  CString value = defValue;
  BOOL isIndeterminate = FALSE;

  hr = settings.GetValue(section, key, value, &isIndeterminate, defValue);
  value.TrimLeft(); value.TrimRight();

  if (bSave) {
    CString newValue;
    control.GetWindowText(newValue);

    if (((!isIndeterminate) && (value.Compare(newValue) == 0)) || (isIndeterminate && newValue.IsEmpty())) {
      return S_FALSE;       // do not modify the value
    } else {
      return settings.SetValue(section, key, newValue);
    }
  } else {
    if (isIndeterminate || FAILED(hr)) {
      control.SetWindowText(_T(""));
    } else {
      control.SetWindowText(value);
    }

    control.EnableWindow(SUCCEEDED(hr));              // disable the control if an error occured

    return hr;
  }
}

#if 0

//
// Enables or disables a group of controls based on whether the given
//  boolean setting is true or not.
//
HRESULT VLPUtil::SyncGroup(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  BOOL defState,                                      // default group enable state assumed if none is defined
  LPCTSTR yesValue,                                   // ini value corresponding to an enabled state
  LPCTSTR noValue)                                    // ini value corresponding to a disabled state
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);
  ASSERT(yesValue != NULL);
  ASSERT(noValue != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (bSave) {
    return S_FALSE;
  } else {
    HRESULT hr = S_OK;
    LPCTSTR defValue = defState ? yesValue : noValue; // default value based on default state
    BOOL isIndeterminate = FALSE;
    CString value = defValue;

    hr = settings.GetValue(section, key, value, &isIndeterminate, defValue);
    value.TrimLeft(); value.TrimRight();

    if (value.CollateNoCase(noValue) == 0) {
      CWnd* pGroupWnd = &control, * pLastGroupWnd = NULL;
      CWnd* pParentWnd = control.GetParent();

      ASSERT_VALID(pParentWnd);

      do {
        ASSERT_VALID(pGroupWnd);

        pGroupWnd->EnableWindow(FALSE);
        pLastGroupWnd = pGroupWnd;
        pGroupWnd = pParentWnd->GetNextDlgGroupItem(pGroupWnd);
      } while((pGroupWnd->GetSafeHwnd() != NULL) && (pGroupWnd->GetSafeHwnd() != pLastGroupWnd->GetSafeHwnd()) && (pGroupWnd->GetSafeHwnd() != control.GetSafeHwnd()));
    }

    return hr;
  }
}

#endif

//
// Loads a Win32 icon from a setting.
//
HRESULT VLPUtil::LoadIconCtl(
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CStatic_Icon& control,                              // static control with which the data must be synchronized
  LPCTSTR defValue)                                   // default value
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  BOOL isIndeterminate = FALSE;
  CString iconLocation;

  HINSTANCE hShInstance = ::GetModuleHandle(_T("SHELL32"));

  if (FAILED(settings.GetValue(section, key, iconLocation, &isIndeterminate, defValue)))
    return control.LoadIcon(hShInstance, MAKEINTRESOURCE(IDI_SH_UNKNOWN));

  if (isIndeterminate)
    return control.LoadIcon(hShInstance, MAKEINTRESOURCE(IDI_SH_MULTISEL));

  CString iconPath;
  int iconIndex;

  ParseIconLocation(iconLocation, iconPath, iconIndex);

  if (FAILED(control.ExtractIcon(AfxGetInstanceHandle(), (LPCTSTR)iconPath, iconIndex)))
    return control.LoadIcon(hShInstance, MAKEINTRESOURCE(IDI_SH_UNKNOWN));

  return S_OK;
}

//
// Loads and scales an icon into a true-color DIB
//
HRESULT VLPUtil::LoadDIBFromIcon(
  CDIBitmap& bmp,
  HINSTANCE hInstance,
  LPCTSTR lpIconName,
  HBRUSH hBgBrush,
  UINT cx,
  UINT cy,
  BOOL stretch,
  UINT cx1,
  UINT cy1)
{
  CDC memDC;
  CBrush bgBrush;
  HICON hIcon;
  HRESULT hr = S_OK;

  if (stretch) {
    cx1 = cx;
    cy1 = cy;
  }

  if ((HBITMAP)bmp != NULL)
    bmp.DeleteObject();

  if (!bmp.CreateDIBitmap(cx, cy, 24, 0))
    return HRESULT_FROM_WIN32(GetLastError());

  if (!memDC.CreateCompatibleDC(NULL))
    return HRESULT_FROM_WIN32(GetLastError());

  CBitmap* oldBmp = memDC.SelectObject(&bmp);

  if (!bgBrush.Attach(hBgBrush))
    return E_HANDLE;

  memDC.FillRect(CRect(0, 0, cx, cy), &bgBrush);

  bgBrush.Detach();

  if ((hIcon = (HICON)::LoadImage(hInstance, lpIconName, IMAGE_ICON, cx1, cy1, LR_DEFAULTCOLOR)) == NULL)
    return HRESULT_FROM_WIN32(GetLastError());

  if (!::DrawIconEx(memDC.m_hDC, ((int)cx - (int)cx1) / 2, ((int)cy - (int)cy1) / 2, hIcon, cx1, cy1, 0, NULL, DI_NORMAL))
    hr = HRESULT_FROM_WIN32(GetLastError());

  if (!::DestroyIcon(hIcon))
    hr = HRESULT_FROM_WIN32(GetLastError());

  VERIFY(memDC.SelectObject(oldBmp)->GetSafeHandle() == bmp.m_hObject);

  return hr;
}

//
// Parses a file location string for its file component and icon index
//
void VLPUtil::ParseIconLocation(
  LPCTSTR iconLocation,         // string that will be parsed
  CString& iconPath,            // file component
  int& iconIndex)               // icon index
{
  iconPath = iconLocation;
  LPTSTR iconPathBuf = iconPath.GetBuffer(0);

  try {
    iconIndex = PathParseIconLocation(iconPathBuf);
    iconPath.ReleaseBuffer();
  } catch (...) {
    iconPath.ReleaseBuffer();
    CString lhs, rhs;
    StrSplit(iconPath, _T(','), lhs, rhs, FALSE, FALSE);
    iconPath = lhs;
    iconIndex = _tcstol(rhs, NULL, 0);
  }
}

//
// Creates a relative path from two paths
//
CString VLPUtil::GetRelativePath(
  LPCTSTR filePath,             // string that contains the path that relPath will be relative to; the relative path will point to this path
  LPCTSTR baseDir,              // string that contains the path that the returned path will be relative from; this is the path in which the relative path can be used
  BOOL isPathOnly)              // if TRUE, filePath is assumed to be a directory; otherwise, filePath is assumed to be a file
{
  ASSERT(filePath != NULL);
  ASSERT(baseDir != NULL);
  ASSERT(_tcslen(filePath) < MAX_PATH);
  ASSERT(_tcslen(baseDir) < MAX_PATH);

  TCHAR szPath[MAX_PATH + 1];

  try {
    if (!IsRelativePath(baseDir) &&
        !IsRelativePath(filePath) &&
        PathRelativePathTo(szPath, baseDir, FILE_ATTRIBUTE_DIRECTORY, filePath, isPathOnly ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL))
    {
      return CString(szPath);
    }
  } catch (...) {
    // Do nothing
  }

  return filePath;
}

//
// Creates an absolute path from two paths
//
CString VLPUtil::GetAbsolutePath(
  LPCTSTR filePath,             // string that contains the relative path to be converted
  LPCTSTR baseDir,              // string that contains the path relative to which the return path is computed from
  BOOL isPathOnly)              // if TRUE, filePath is assumed to be a directory; otherwise, filePath is assumed to be a file
{
  ASSERT(filePath != NULL);
  ASSERT(baseDir != NULL);
  ASSERT(_tcslen(filePath) < MAX_PATH);
  ASSERT(_tcslen(baseDir) < MAX_PATH);

  TCHAR szPath1[MAX_PATH + MAX_PATH + 1], szPath2[MAX_PATH + MAX_PATH + 1];

  _tcscpy(szPath1, baseDir);

  try {
    if (!IsRelativePath(baseDir) &&
        IsRelativePath(filePath) &&
        PathAppend(szPath1, filePath) &&
        PathCanonicalize(szPath2, szPath1))
    {
      return CString(szPath2);
    }
  } catch (...) {
    // Do nothing
  }

  return filePath;
}

//
// Removes the trailing file name and backslash from a path, if it has them
//
CString VLPUtil::GetDirectory(LPCTSTR filePath) {
  try {
    CString retVal(filePath);
    PathRemoveFileSpec(retVal.GetBuffer(0));
    retVal.ReleaseBuffer();
    return retVal;
  } catch (...) {
    CString lhs, rhs;
    StrSplit(filePath, _T('\\'), lhs, rhs, FALSE, FALSE);
    return lhs;
  }
}

//
// Returns the filename by removing any diectory or drive information
//
CString VLPUtil::GetFilename(LPCTSTR filePath, BOOL removeExtension) {
  try {
    CString retVal = PathFindFileName(filePath);

    if (removeExtension) {
      PathRemoveExtension(retVal.GetBuffer(0));
      retVal.ReleaseBuffer();
    }

    return retVal;
  } catch (...) {
    CString lhs, rhs;
    StrSplit(filePath, _T('\\'), lhs, rhs, FALSE, TRUE);

    if (removeExtension) {
      StrSplit(lhs, _T('.'), lhs, rhs, FALSE, FALSE);
    }

    return lhs;
  }
}

//
// Verifies that a path is a valid directory
//
BOOL VLPUtil::IsDirectory(LPCTSTR pszPath) {
  try {
    return PathIsDirectory(pszPath);
  } catch (...) {
    return (GetFileAttributes(pszPath) & FILE_ATTRIBUTE_DIRECTORY);
  }
}

//
// Searches a path and determines if it is relative
//
BOOL VLPUtil::IsRelativePath(LPCTSTR pszPath) {
  return PathIsRelative(pszPath);
}

//
//
//
CString VLPUtil::RenameExtension(LPCTSTR filePath, LPCTSTR ext) {
  ASSERT(filePath != NULL);
  ASSERT(ext != NULL);
  ASSERT(_tcslen(filePath) < MAX_PATH);
  ASSERT(_tcslen(ext) < MAX_PATH);

  TCHAR szPath1[MAX_PATH + 1];

  _tcscpy(szPath1, filePath);

  try {
    return PathRenameExtension(szPath1, ext) ? szPath1 : _T("");
  } catch (...) {
    TCHAR* split = _tcsrchr(szPath1, _T('.'));

    if (split != NULL) {
      *split = _T('\0');
    } else {
      split = &(szPath1[_tcslen(szPath1)]);
    }

    _tcscat(szPath1, ext);

    return szPath1;
  }
}

//
//
//
BOOL VLPUtil::FileExists(LPCTSTR filePath) {
  if (filePath[0] == _T('\0'))
    return FALSE;

  try {
    return PathFileExists(filePath);
  } catch (...) {
    HANDLE hFile = CreateFile(filePath, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile !=  INVALID_HANDLE_VALUE) {
      CloseHandle(hFile);
      return TRUE;
    } else {
      return FALSE;
    }
  }
}

//
//
//
HRESULT VLPUtil::GetSpecialFolderPath(CString& result, int nFolder, HWND hwndOwner) {
  HRESULT hr;

  try {
    LPTSTR szResult = result.GetBuffer(MAX_PATH);

    ASSERT(szResult != NULL);

    hr = SHGetSpecialFolderPath(hwndOwner, szResult, nFolder, TRUE);
    result.ReleaseBuffer();
    return hr;
  } catch (...) {
    CComPtr<IMalloc> pMalloc;

    if (FAILED(hr = SHGetMalloc(&pMalloc)))
      return hr;

    LPITEMIDLIST pidl;

    if (FAILED(hr = SHGetSpecialFolderLocation(NULL, nFolder, &pidl)))
      return hr;

    LPTSTR szResult = result.GetBuffer(MAX_PATH);

    ASSERT(szResult != NULL);

    if (!SHGetPathFromIDList(pidl, szResult)) {
      result.ReleaseBuffer();
      return E_FAIL;
    }

    result.ReleaseBuffer();

    pMalloc->Free(pidl);

    return S_OK;
  }
}

//
// Expands any environment variables in the given path
//
CString VLPUtil::GetExpandedPath(LPCTSTR filePath) {
  CString retVal;

  LPTSTR szRetVal = retVal.GetBuffer(MAX_PATH);

  ASSERT(szRetVal != NULL);

  DWORD nSize = ExpandEnvironmentStrings(filePath, szRetVal, MAX_PATH);
  retVal.ReleaseBuffer();

  if ((nSize < 1) || (nSize > MAX_PATH)) {
    return CString(filePath);
  } else {
    return retVal;
  }
}

//
//
//
CString VLPUtil::GetShortPath(LPCTSTR fileLongPath) {
  CString retVal;

  LPTSTR szRetVal = retVal.GetBuffer(MAX_PATH);

  ASSERT(szRetVal != NULL);

  DWORD nSize = GetShortPathName(fileLongPath, szRetVal, MAX_PATH);
  retVal.ReleaseBuffer();

  if ((nSize < 1) || (nSize > MAX_PATH)) {
    return CString(fileLongPath);
  } else {
    return retVal;
  }
}

//
//
//
HRESULT VLPUtil::EnableAutoComplete(HWND hwndEdit) {
  try {
    return SHAutoComplete(hwndEdit, SHACF_FILESYSTEM);
  } catch (...) {
    return E_NOTIMPL;
  }
}

//
//
//
BOOL VLPUtil::BrowseForFile(CString& fileName, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) {
  BOOL retVal = FALSE;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  CFileDialog dlgFile(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd);

  switch (dlgFile.DoModal()) {
    case IDOK:
      fileName = dlgFile.GetPathName();
      retVal = TRUE;
      break;

    case IDCANCEL:
      break;

    default:
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      if (pParentWnd) pParentWnd->GetWindowText(strTmp2);
      MessageBox(pParentWnd->GetSafeHwnd(), strTmp1, strTmp2, MB_OK | MB_ICONERROR);
  }

  return retVal;
}

//
//
//
BOOL VLPUtil::BrowseForFolder(CString& folderName, UINT ulFlags, LPCTSTR lpszTitle, LPCITEMIDLIST pidlRoot, CWnd* pParentWnd) {
  BOOL retVal = FALSE;

  DWORD lastError = ERROR_SUCCESS;
  CString strTmp1, strTmp2;

  TCHAR tmpBuf[MAX_PATH];

  BROWSEINFO browseInfo;
  browseInfo.hwndOwner = pParentWnd->GetSafeHwnd();
  browseInfo.pidlRoot  = pidlRoot;
  browseInfo.pszDisplayName = tmpBuf;
  browseInfo.lpszTitle = lpszTitle;
  browseInfo.ulFlags   = ulFlags;
  browseInfo.lpfn      = NULL;
  browseInfo.lParam    = 0;
  browseInfo.iImage    = 0;

  LPITEMIDLIST pidlBrowse = SHBrowseForFolder(&browseInfo);

  if (pidlBrowse != NULL) {
    if (SHGetPathFromIDList(pidlBrowse, tmpBuf)) {
      folderName = tmpBuf;
      retVal = TRUE;
    } else {
      lastError = GetLastError();
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, lastError, (LPCTSTR)VLPUtil::FormatMessage(lastError, true, NULL));
      if (pParentWnd) pParentWnd->GetWindowText(strTmp2);
      MessageBox(pParentWnd->GetSafeHwnd(), strTmp1, strTmp2, MB_OK | MB_ICONERROR);
    }

    HRESULT hr;
    CComPtr<IMalloc> pMalloc;

    if (FAILED(hr = SHGetMalloc(&pMalloc))) {
      strTmp1.FormatMessage(IDS_MSG_UNEXPECTEDERR, hr, (LPCTSTR)VLPUtil::FormatMessage(hr, true, NULL));
      if (pParentWnd) pParentWnd->GetWindowText(strTmp2);
      MessageBox(pParentWnd->GetSafeHwnd(), strTmp1, strTmp2, MB_OK | MB_ICONERROR);
    } else {
      pMalloc->Free(pidlBrowse);
    }
  }

  return retVal;
}

//
//
//
BOOL VLPUtil::isVLPFile(LPCTSTR fName) {
  try {
    return (_tcsicmp(PathFindExtension(fName), _T(".vlp")) == 0) && !PathIsDirectory(fName);
  } catch (...) {
    CString lhs, rhs;
    StrSplit(fName, _T('.'), lhs, rhs, FALSE, FALSE);
    return (rhs.CompareNoCase(_T("vlp")) == 0);
  }
}

//
//
//
BOOL VLPUtil::isMSDOSFile(LPCTSTR fName) {
  try {
    return (_tcsicmp(PathFindExtension(fName), _T(".bat")) == 0) || (SHGetFileInfo(fName, 0, NULL, 0, SHGFI_EXETYPE) == 0x00005a4d);
  } catch (...) {
    CString lhs, rhs;
    StrSplit(fName, _T('.'), lhs, rhs, FALSE, FALSE);
    return (rhs.CompareNoCase(_T("exe")) == 0) || (rhs.CompareNoCase(_T("com")) == 0) || (rhs.CompareNoCase(_T("bat")) == 0);
  }
}

//
//
//
CString VLPUtil::GetVDMSFilePath(LPCTSTR fileName) {
  LPCTSTR szVDMSPath = _tgetenv(_T("VDMSPath"));
  ASSERT(szVDMSPath != NULL);

  if (szVDMSPath != NULL) {
    return GetAbsolutePath(fileName, szVDMSPath);
  } else {
    return fileName;
  }
}

//
//
//
HRESULT VLPUtil::GetEffectiveRights(
  LPCTSTR pObjectName,          // address of string for file name
  SE_OBJECT_TYPE ObjectType,    // type of object
  PACCESS_MASK pAccessRights)   // receives trustee's access rights
{
  ASSERT(pObjectName != NULL);
  ASSERT(pAccessRights != NULL);

  HRESULT hr;
  PSID psidOwner;
  PACL pDacl = NULL;
  PSECURITY_DESCRIPTOR pPrcSD = NULL;
  PSECURITY_DESCRIPTOR pObjSD = NULL;

  try {
    DWORD err;

    if ((err = GetNamedSecurityInfo(const_cast<LPTSTR>(pObjectName), ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, &pDacl, NULL, &pObjSD)) != ERROR_SUCCESS)
      throw err;

    if ((err = GetSecurityInfo(GetCurrentProcess(), SE_KERNEL_OBJECT, OWNER_SECURITY_INFORMATION, &psidOwner, NULL, NULL, NULL, &pPrcSD)) != ERROR_SUCCESS)
      throw err;

    TRUSTEE trustee;
    BuildTrusteeWithSid(&trustee, psidOwner);

    if ((err = GetEffectiveRightsFromAcl(pDacl, &trustee, pAccessRights)) != ERROR_SUCCESS)
      throw err;
  } catch (DWORD err) {
    hr = HRESULT_FROM_WIN32(err);
  } catch (...) {
    hr = HRESULT_FROM_WIN32(ERROR_DLL_NOT_FOUND);
  }

  if (pPrcSD != NULL)
    LocalFree((HLOCAL)pPrcSD);

  if (pObjSD != NULL)
    LocalFree((HLOCAL)pObjSD);

  return hr;
}

//
//
//
CString AFX_CDECL VLPUtil::FormatMessage(
    DWORD dwMessageID,          // index into message-table resource where the message definition can be found
    bool bFromSystem,           // use the system message-table resources instead of application-provided ones to resolve GetLastError() codes
    HMODULE hModule,            // module containing the message-table resources; NULL means current process' application image
    DWORD dwLanguageID,         // language to be used; use 0 or MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) for default
    bool bIgnoreInserts,        // ignore the "%..." inserts in the message definition; useful if no arguments can be provided, or if formatting is to be performed later
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
CString AFX_CDECL VLPUtil::FormatString(
    LPCTSTR lpszFormat,         // A format-control string.
    ... )
{
  CString Buffer;
  va_list Arguments;
  va_start(Arguments, lpszFormat);
  Buffer.FormatV(lpszFormat, Arguments);
  va_end(Arguments);
  return Buffer;
}

//
//
//
CString VLPUtil::LoadString(
  UINT nID)
{
  CString result;
  result.LoadString(nID);
  return result;
}
