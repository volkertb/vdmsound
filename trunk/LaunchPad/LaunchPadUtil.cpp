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
//
// CIcon
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIcon::CIcon(void) : CStatic(), m_hIcon(NULL), m_bAttached(FALSE) {
}

CIcon::~CIcon(void) {
  if (m_bAttached)
    DeleteIcon();

  CStatic::~CStatic();
}



//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

HRESULT CIcon::LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName) {
  HICON hIcon;

  if (m_bAttached)
    DeleteIcon();

  if ((hIcon = ::LoadIcon(hInstance, lpIconName)) == NULL)
    return HRESULT_FROM_WIN32(GetLastError());

  m_hIcon = hIcon;
  m_bAttached = FALSE;

  SetIcon(hIcon);

  return S_OK;
}

HRESULT CIcon::ExtractIcon(HINSTANCE hInstance, LPCTSTR lpszExeFileName, UINT nIconIndex) {
  HICON hIcon;

  if (m_bAttached)
    DeleteIcon();

  if ((hIcon = ::ExtractIcon(hInstance, lpszExeFileName, nIconIndex)) < (HICON)2)
    return E_FAIL;

  m_hIcon = hIcon;
  m_bAttached = TRUE;

  SetIcon(hIcon);

  return S_OK;
}

BOOL CIcon::DeleteIcon(void) {
	if (m_hIcon == NULL)
		return FALSE;

  SetIcon(NULL);

  BOOL retVal = ::DestroyIcon(m_hIcon);

  m_hIcon = NULL;
  m_bAttached = FALSE;

  return retVal;
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
  m_ofn.lpstrFilter  = _T("Programs (*.exe,*.com)\0*.exe;*.com\0Batch files (*.bat)\0*.bat\0All Files (*.*)\0*.*\0\0");
  m_ofn.nFilterIndex = 1;

}



//////////////////////////////////////////////////////////////////////
// Overridables
//////////////////////////////////////////////////////////////////////

BOOL COpenDOSProgramDialog::OnFileNameOK(void) {
  // Check that the file is a MS-DOS .exe, .com, or .bat file
  if ((GetFileExt().CompareNoCase(_T("BAT")) == 0) ||                                     // .bat file
      (SHGetFileInfo((LPCTSTR)GetPathName(), 0, NULL, 0, SHGFI_EXETYPE) == 0x00005a4d))   // valid .exe/.com file
  {
    return FALSE;
  } else {
    CString message;
    message.Format(_T("The file '%s' does not appear to be a valid MS-DOS executable or batch file.\n\rDo you want to continue?"), (LPCTSTR)GetFileName());
    return (MessageBox(message, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES);
  }
}



//////////////////////////////////////////////////////////////////////
//
// LaunchPadSettingsHelper
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////

LPCTSTR LaunchPadSettingsHelper::T_YES = _T("yes");
LPCTSTR LaunchPadSettingsHelper::T_NO = _T("no");



//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
// Synchronizes the state of a Win32 check-box with its corresponding
//  setting
//
HRESULT LaunchPadSettingsHelper::SyncCheckBox(
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
HRESULT LaunchPadSettingsHelper::SyncRadioButton(
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
HRESULT LaunchPadSettingsHelper::SyncRadioButton(
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
HRESULT LaunchPadSettingsHelper::LoadIconCtl(
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CIcon& control)                                     // static control with which the data must be synchronized
{
  ASSERT(section != NULL);
  ASSERT(key != NULL);

  ASSERT_VALID(&control);

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  BOOL isIndeterminate = FALSE;
  CString iconLocation;

  HINSTANCE hInstance = _Module.GetModuleInstance();

  if (FAILED(settings.GetValue(section, key, iconLocation, &isIndeterminate, _T(""))))
    return control.LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNKNOWNICON));

  if (isIndeterminate)
    return control.LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MULTIPLEICON));

  CString iconPath;
  int iconIndex;

  ParseIconLocation(iconLocation, iconPath, iconIndex);

  if (FAILED(control.ExtractIcon(hInstance, (LPCTSTR)iconPath, iconIndex)))
    return control.LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNKNOWNICON));

  return S_OK;
}

//
// Parses a file location string for its file component and icon index
//
void LaunchPadSettingsHelper::ParseIconLocation(
  LPCTSTR iconLocation,         // string that will be parsed
  CString& iconPath,            // file component
  int& iconIndex)               // icon index
{
  iconPath = iconLocation;
  LPTSTR iconPathBuf = iconPath.LockBuffer();

  iconIndex = PathParseIconLocation(iconPathBuf);

  iconPath.UnlockBuffer();
}

//
// Creates a relative path from two paths
//
CString LaunchPadSettingsHelper::GetRelativePath(
  LPCTSTR filePath,             // string that contains the path that relPath will be relative to; the relative path will point to this path
  BOOL isPathOnly,              // if TRUE, filePath is assumed to be a directory; otherwise, filePath is assumed to be a file
  LPCTSTR basePath)             // string that contains the path that relPath will be relative from; this is the path in which the relative path can be used
{
  TCHAR szPath[MAX_PATH];

  if ((!PathIsRelative(filePath)) &&
      (PathIsDirectory(basePath)) &&
      (PathRelativePathTo(szPath, basePath, FILE_ATTRIBUTE_DIRECTORY, filePath, isPathOnly ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL)))
  {
    return CString(szPath);
  } else {
    return filePath;
  }
}

//
// Removes the trailing file name and backslash from a path, if it has them
//
CString LaunchPadSettingsHelper::GetDirectory(LPCTSTR filePath) {
  CString retVal(filePath);
  LPTSTR retValBuf = retVal.LockBuffer();
  PathRemoveFileSpec(retValBuf);
  retVal.UnlockBuffer();

  return retVal;
}
