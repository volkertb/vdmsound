// LaunchPadShellEx.cpp : Implementation of CLaunchPadShellEx
#include "stdafx.h"
#include "LaunchPad.h"
#include "LaunchPadShellEx.h"

#include "BasicSettingsPage.h"

#include "RUNWITHVDMSDispatcher.h"

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CLaunchPadShellEx

/////////////////////////////////////////////////////////////////////////////
// IShellExtInit
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

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
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Only display the property page if all files are of type "VDMSound LaunchPad"
    for (int i = 0; i < m_fileNames.GetSize(); i++) {
      if (!VLPUtil::isVLPFile(m_fileNames.GetAt(i)))
        return S_OK;    // return right away, without displaying the property page
    }

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
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (idCmd != 0)
      return E_INVALIDARG;

    switch (uFlags) {
      case GCS_HELPTEXTA:
        LoadStringA(AfxGetResourceHandle(), IDS_TXT_RUNWITHVDMSHLP, (LPSTR)pszName, cchMax);
        return S_OK;

      case GCS_HELPTEXTW:
        LoadStringW(AfxGetResourceHandle(), IDS_TXT_RUNWITHVDMSHLP, (LPWSTR)pszName, cchMax);
        return S_OK;

      case GCS_VALIDATEA:
      case GCS_VALIDATEW:
        return S_OK;

      default:
        return E_FAIL;
    }
  } catch (...) {
    return E_UNEXPECTED;
  }
}

HRESULT CLaunchPadShellEx::InvokeCommand(LPCMINVOKECOMMANDINFO lpici) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (HIWORD(lpici->lpVerb) != 0)
      return E_INVALIDARG;

    ASSERT(m_fileNames.GetSize() == 1);

    if (m_fileNames.GetSize() != 1)
      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    CRUNWITHVDMSDispatcher dispatcher(m_fileNames.GetAt(0));

    switch (LOWORD(lpici->lpVerb)) {
      case 0:
        return dispatcher.RunWithVdms();
      default:
        return E_INVALIDARG;
    }
  } catch (...) {
    return E_UNEXPECTED;
  }
}

HRESULT CLaunchPadShellEx::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USHORT numMenuItems = 0;

    if (idCmdFirst > idCmdLast)
      return E_INVALIDARG;

    if (hMenu == NULL)
      return E_INVALIDARG;

    if (m_fileNames.GetSize() != 1)
      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    if (!VLPUtil::isVLPFile(m_fileNames.GetAt(0)) && !VLPUtil::isMSDOSFile(m_fileNames.GetAt(0)))
      return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    int chkW = GetSystemMetrics(SM_CXMENUCHECK);
    int chkH = GetSystemMetrics(SM_CYMENUCHECK);

    int icoW = (int)(chkW / 16.0 + 0.5) * 16;
    int icoH = (int)(chkH / 16.0 + 0.5) * 16;

    ASSERT((HBITMAP)m_contextMenuBmp == NULL);

    VLPUtil::LoadDIBFromIcon(m_contextMenuBmp, AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2), CBrush(0xffffff), chkW, chkH, FALSE, icoW, icoH);

    if (InsertMenu(hMenu, indexMenu, MF_BYPOSITION, idCmdFirst + numMenuItems, VLPUtil::LoadString(IDS_TXT_RUNWITHVDMS))) {
      SetMenuItemBitmaps(hMenu, indexMenu, MF_BYPOSITION, m_contextMenuBmp, m_contextMenuBmp);
      numMenuItems++;
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, numMenuItems);
  } catch (...) {
    return E_UNEXPECTED;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IPersistFile
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Load(LPCOLESTR pszFileName, DWORD dwMode) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ASSERT(m_fileNames.GetSize() == 0);

    m_fileNames.Add(pszFileName);

    return S_OK;
  } catch (...) {
    return E_UNEXPECTED;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IExtractIcon
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Extract(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize) {
  return S_FALSE;
}

HRESULT CLaunchPadShellEx::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, LPINT piIndex, UINT* pwFlags) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ASSERT(m_fileNames.GetSize() == 1);

    CString iconLocation, iconPath;
    int iconIndex;

    if ((szIconFile == NULL) || (piIndex == NULL) || (pwFlags == NULL))
      return E_POINTER;

    if (cchMax <= 0)
      return E_INVALIDARG;

    CLaunchPadSettings settings(m_fileNames);

    if (FAILED(settings.GetValue(_T("program"), _T("icon"), iconLocation)))
      return S_FALSE;

    VLPUtil::ParseIconLocation(iconLocation, iconPath, iconIndex);

    _tcsncpy(szIconFile, (LPCTSTR)iconPath, cchMax);

    *piIndex = iconIndex;
    *pwFlags = GIL_PERINSTANCE;

    return S_OK;
  } catch (...) {
    return E_UNEXPECTED;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IQueryInfo
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip) {
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;

    *ppwszTip = NULL;

    CComPtr<IMalloc> pMalloc;

    if (FAILED(SHGetMalloc(&pMalloc)))
      return E_FAIL;

    ASSERT(m_fileNames.GetSize() == 1);

    CLaunchPadSettings settings(m_fileNames);

    CString progExec, progParams, progWDir;
    CString strTmp, strToolTip;

    settings.GetValue(_T("program"), _T("executable"), progExec);
    settings.GetValue(_T("program"), _T("params"), progParams);
    settings.GetValue(_T("program"), _T("workdir"), progWDir);

    strTmp = VLPUtil::GetAbsolutePath(progExec, progWDir, FALSE) + _T(" ") + progParams;
    strTmp.TrimLeft(); strTmp.TrimRight();

    strToolTip.FormatMessage(IDS_MSG_SHTOOLTIP, (LPCTSTR)strTmp);

    if ((*ppwszTip = (LPWSTR)pMalloc->Alloc((strToolTip.GetLength() + 1) * sizeof(wchar_t))) == NULL)
      return E_OUTOFMEMORY;

    wcscpy(*ppwszTip, T2COLE((LPCTSTR)strToolTip));

    return S_OK;
  } catch (...) {
    return E_UNEXPECTED;
  }
}
