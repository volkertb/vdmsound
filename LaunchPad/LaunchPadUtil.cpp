#include "stdafx.h"
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
    return HRESULT_FROM_WIN32(GetLastError());

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
  BOOL isSetOp,                                       // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  BOOL defState,                                      // default button state assumed if none is defined
  LPCTSTR yesValue,                                   // ini value corresponding to a checked state
  LPCTSTR noValue)                                    // ini value corresponding to an unchecked state
{
  _ASSERTE(section != NULL);
  _ASSERTE(key != NULL);
  _ASSERTE(yesValue != NULL);
  _ASSERTE(noValue != NULL);

  control.AssertValid();

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (isSetOp) {
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
  BOOL isSetOp,                                       // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  BOOL defState,                                      // default button state assumed if none is defined
  LPCTSTR selValue)                                   // ini value corresponding to a selected state
{
  _ASSERTE(section != NULL);
  _ASSERTE(key != NULL);
  _ASSERTE(selValue != NULL);

  control.AssertValid();

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (isSetOp) {
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
  BOOL isSetOp,                                       // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CButton& control,                                   // button control with which the data must be synchronized
  LPCTSTR selValue1,                                  // list of known values for which this button should not be in a selected state
  ...)                                                //  (list must end with a NULL value)
{
  _ASSERTE(section != NULL);
  _ASSERTE(key != NULL);
  _ASSERTE(selValue1 != NULL);

  control.AssertValid();

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (isSetOp) {
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
// Synchronizes the state of a Win32 icon with its corresponding
//  setting.
//
HRESULT LaunchPadSettingsHelper::SyncIcon(
  BOOL isSetOp,                                       // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CIcon& control)                                     // static control with which the data must be synchronized
{
  _ASSERTE(section != NULL);
  _ASSERTE(key != NULL);

  control.AssertValid();

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  if (isSetOp) {
    return S_OK;
  } else {
    HRESULT hr = S_OK;
    BOOL isIndeterminate = FALSE;
    CString value;

    hr = settings.GetValue(section, key, value, &isIndeterminate, _T(""));

    if (FAILED(hr)) {
      return control.LoadIcon(NULL, MAKEINTRESOURCE(IDI_WINLOGO));
    } else if (isIndeterminate) {
      return control.LoadIcon(NULL, MAKEINTRESOURCE(IDI_QUESTION));
    } else {
      HINSTANCE hInstance = _Module.GetModuleInstance();

      if (SUCCEEDED(control.ExtractIcon(hInstance, (LPCTSTR)value, 0)))
        return S_OK;

      int splitPos = value.ReverseFind(_T(','));

      if (splitPos < 0)
        return E_FAIL;

      return control.ExtractIcon(hInstance, (LPCTSTR)value.Left(splitPos), _tcstol((LPCTSTR)value.Mid(splitPos + 1), NULL, 10));
    }

    return E_NOTIMPL;
  }
}

void LaunchPadSettingsHelper::ParseIconString(
  const CString& iconString,
  CString& iconPath,
  int& iconIndex)
{
  int splitPos = iconString.ReverseFind(_T(','));

  if (splitPos < 0) {
    iconPath = iconString;
    iconIndex = 0;
  } else {
    
  }
}
