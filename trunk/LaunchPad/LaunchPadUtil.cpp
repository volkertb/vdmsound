#include "stdafx.h"
#include "resource.h"
#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

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
: CFileDialog(TRUE, NULL, lpszFileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, NULL, pParentWnd)
{
  m_strFilter.LoadString(IDS_TXT_FILTER1);
  m_strFilter += _T('|'); // add trailing delimiter

  // Change '|' delimiters to '\0'; do not call ReleaseBuffer() since the string contains '\0' characters
  for (LPTSTR pch = m_strFilter.GetBuffer(0); (pch = _tcschr(pch, '|')) != NULL; *(pch++) = '\0');

  m_ofn.lpstrFilter  = m_strFilter;
  m_ofn.nFilterIndex = 1;
}

//////////////////////////////////////////////////////////////////////
// Overridables
//////////////////////////////////////////////////////////////////////

BOOL COpenDOSProgramDialog::OnFileNameOK(void) {
  // Check that the file is a MS-DOS .exe, .com, or .bat file
  if (VLPUtil::isMSDOSFile(GetPathName())) {  // valid .exe/.com file
    return FALSE;
  } else {
    CString message;
    message.Format(_T("The file '%s' does not appear to be a valid MS-DOS executable or batch file.\n\rDo you want to continue?"), (LPCTSTR)GetFileName());
    return (MessageBox(message, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES);
  }
}



//////////////////////////////////////////////////////////////////////
//
// VLPUtil
//
//////////////////////////////////////////////////////////////////////

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
    if ((!control.IsWindowEnabled()) || (control.GetState() == BST_INDETERMINATE))
      return settings.UnsetValue(section, key);

    switch (control.GetState()) {
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

    control.EnableWindow(SUCCEEDED(hr));              // disable the control if an error occured

    int bState;

    if (isIndeterminate) {
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
    if ((control.IsWindowEnabled()) && (control.GetState() == BST_CHECKED)) {
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

    control.EnableWindow(SUCCEEDED(hr));              // disable the control if an error occured

    int bState;

    if (isIndeterminate) {
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
    if ((control.IsWindowEnabled()) && (control.GetState() == BST_CHECKED)) {
      return settings.UnsetValue(section, key);
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

    BOOL isEnabled = SUCCEEDED(hr);                   // disable the control if an error occured

    if (!isIndeterminate) {
      for (LPCTSTR arg = selValue1; arg != NULL; arg = va_arg(argList, LPCTSTR)) {
        if (value.CollateNoCase(arg) == 0) {
          isEnabled = FALSE;
          break;
        }
      }
    }

    control.EnableWindow(isEnabled);
    control.SetButtonStyle(BS_AUTORADIOBUTTON);
    control.SetCheck(isEnabled ? BST_CHECKED : BST_UNCHECKED);

    va_end(argList);

    return hr;
  }
}

//
// Loads a Win32 icon from a setting.
//
HRESULT VLPUtil::LoadIconCtl(
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CStatic_Icon& control)                              // static control with which the data must be synchronized
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  BOOL isIndeterminate = FALSE;
  CString iconLocation;

  HINSTANCE hShInstance = ::GetModuleHandle(_T("SHELL32"));

  if (FAILED(settings.GetValue(section, key, iconLocation, &isIndeterminate, _T(""))))
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

  ::DrawIconEx(memDC.m_hDC, ((int)cx - (int)cx1) / 2, ((int)cy - (int)cy1) / 2, hIcon, cx1, cy1, 0, NULL, DI_NORMAL);
  ::DestroyIcon(hIcon);

  memDC.SelectObject(oldBmp);

  return S_OK;
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

  iconIndex = PathParseIconLocation(iconPathBuf);

  iconPath.ReleaseBuffer();
}

//
// Creates a relative path from two paths
//
CString VLPUtil::GetRelativePath(
  LPCTSTR filePath,             // string that contains the path that relPath will be relative to; the relative path will point to this path
  BOOL isPathOnly,              // if TRUE, filePath is assumed to be a directory; otherwise, filePath is assumed to be a file
  LPCTSTR basePath)             // string that contains the path that the returned path will be relative from; this is the path in which the relative path can be used
{
  ASSERT(filePath != NULL);
  ASSERT(basePath != NULL);
  ASSERT(_tcslen(filePath) < MAX_PATH);
  ASSERT(_tcslen(basePath) < MAX_PATH);

  TCHAR szPath[MAX_PATH + 1];

  if (!PathIsRelative(basePath) &&
      !PathIsRelative(filePath) &&
      PathRelativePathTo(szPath, basePath, FILE_ATTRIBUTE_DIRECTORY, filePath, isPathOnly ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL))
  {
    return CString(szPath);
  } else {
    return filePath;
  }
}

//
// Creates an absolute path from two paths
//
CString VLPUtil::GetAbsolutePath(
  LPCTSTR filePath,             // string that contains the relative path to be converted
  BOOL isPathOnly,              // if TRUE, filePath is assumed to be a directory; otherwise, filePath is assumed to be a file
  LPCTSTR basePath)             // string that contains the path relative to which the return path is computed from
{
  ASSERT(filePath != NULL);
  ASSERT(basePath != NULL);
  ASSERT(_tcslen(filePath) < MAX_PATH);
  ASSERT(_tcslen(basePath) < MAX_PATH);

  TCHAR szPath1[2 * MAX_PATH + 1], szPath2[2 * MAX_PATH + 1];

  _tcscpy(szPath1, basePath);

  if (!PathIsRelative(basePath) &&
      PathIsRelative(filePath) &&
      PathAppend(szPath1, filePath) &&
      PathCanonicalize(szPath2, szPath1))
  {
    return CString(szPath2);
  } else {
    return filePath;
  }
}

//
// Removes the trailing file name and backslash from a path, if it has them
//
CString VLPUtil::GetDirectory(LPCTSTR filePath) {
  CString retVal(filePath);

  LPTSTR retValBuf = retVal.GetBuffer(0);

  PathRemoveFileSpec(retValBuf);

  retVal.ReleaseBuffer();

  return retVal;
}

//
//
//
BOOL VLPUtil::isVLPFile(LPCTSTR fName) {
  return (_tcsicmp(PathFindExtension(fName), _T(".vlp")) == 0);
}

//
//
//
BOOL VLPUtil::isMSDOSFile(LPCTSTR fName) {
  return (_tcsicmp(PathFindExtension(fName), _T(".bat")) == 0) || (SHGetFileInfo(fName, 0, NULL, 0, SHGFI_EXETYPE) == 0x00005a4d);
}
