// LaunchPadShellEx.cpp : Implementation of CLaunchPadShellEx
#include "stdafx.h"
#include "LaunchPad.h"
#include "LaunchPadShellEx.h"

#include "BasicSettingsPage.h"

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CLaunchPadShellEx

/////////////////////////////////////////////////////////////////////////////
// IShellExtInit
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  try {
    FORMATETC format = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM storage;
    HDROP     hDrop;

    UINT      uNumFiles;
    TCHAR     szFileName[MAX_PATH];

    // Read the list of items from the data object.  They are stored
    //  in HDROP form, so just get the HDROP handle and then use the
    //  drag'n'drop API on it
    if (FAILED(pDataObj->GetData(&format, &storage)))
      return E_INVALIDARG;

    // Get a HDROP handle
    hDrop = (HDROP)GlobalLock(storage.hGlobal);

    if (!hDrop) {
      ReleaseStgMedium(&storage);
      return E_INVALIDARG;
    }

    // Determine how many files are involved in this operation
    uNumFiles = DragQueryFile(hDrop, (UINT)(-1), NULL, 0);

    for (UINT uFileIdx = 0; uFileIdx < uNumFiles; uFileIdx++) {
      // Get the next filename
      if (DragQueryFile(hDrop, uFileIdx, szFileName, MAX_PATH) == 0)
        continue;

      // Skip over directories
      if (PathIsDirectory(szFileName))
        continue;

      // Add the filename to our list of files to act on
      m_fileNames.Add(szFileName);
    }

    // Release resources
    GlobalUnlock(storage.hGlobal);
    ReleaseStgMedium(&storage);

    // If we found any files we can work with, return S_OK.  Otherwise,
    // return E_FAIL so we don't get called again
    return ((m_fileNames.GetSize() > 0) ? S_OK : E_FAIL);
  } catch (...) {
    return E_UNEXPECTED;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IShellPropSheetExt
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  try {
    // Create and add the property page to the Shell's "Properties" property sheet
    return CBasicSettingsPage::AddPage(lpfnAddPageProc, lParam, m_fileNames);
  } catch (...) {
    return E_UNEXPECTED;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IContextMenu
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: implement
  return E_NOTIMPL;
}

HRESULT CLaunchPadShellEx::InvokeCommand(LPCMINVOKECOMMANDINFO lpici) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: implement
  return E_NOTIMPL;
}

HRESULT CLaunchPadShellEx::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  USHORT numMenuItems = 0;

  if (idCmdFirst > idCmdLast)
    return E_INVALIDARG;

  if (hMenu == NULL)
    return E_INVALIDARG;

  // TODO: use InsertMenuItem and MENUITEMINFO
  InsertMenu(hMenu, indexMenu, MF_BYPOSITION, idCmdFirst + numMenuItems, _T("Run with &VDMS"));
  numMenuItems++;

  return MAKE_HRESULT(SEVERITY_SUCCESS, 0, numMenuItems);
}



/////////////////////////////////////////////////////////////////////////////
// IPersistFile
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Load(LPCOLESTR pszFileName, DWORD dwMode) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  m_fileNames.Add(pszFileName);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IExtractIcon
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Extract(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize) {
  return S_FALSE;
}

HRESULT CLaunchPadShellEx::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, LPINT piIndex, UINT* pwFlags) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString iconLocation, iconPath;
  int iconIndex;

  if ((szIconFile == NULL) || (piIndex == NULL) || (pwFlags == NULL))
    return E_POINTER;

  if (cchMax <= 0)
    return E_INVALIDARG;

  CLaunchPadSettings settings(m_fileNames);

  if (FAILED(settings.GetValue(_T("program"), _T("icon"), iconLocation)))
    return S_FALSE;

  LaunchPadSettingsHelper::ParseIconLocation(iconLocation, iconPath, iconIndex);

  _tcsncpy(szIconFile, (LPCTSTR)iconPath, cchMax);

  *piIndex = iconIndex;
  *pwFlags = GIL_PERINSTANCE;

  return S_OK;
}
