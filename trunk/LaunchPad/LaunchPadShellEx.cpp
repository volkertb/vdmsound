// LaunchPadShellEx.cpp : Implementation of CLaunchPadShellEx
#include "stdafx.h"
#include "LaunchPad.h"
#include "LaunchPadShellEx.h"

/////////////////////////////////////////////////////////////////////////////
// CLaunchPadShellEx

BOOL CALLBACK PropPageDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL OnInitDialog(HWND, LPARAM);
BOOL OnApply(HWND, PSHNOTIFY*);

/////////////////////////////////////////////////////////////////////////////
// IShellExtInit
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID) {
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
}



/////////////////////////////////////////////////////////////////////////////
// IShellPropSheetExt
/////////////////////////////////////////////////////////////////////////////

HRESULT CLaunchPadShellEx::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  PROPSHEETPAGE  pageProps;
  HPROPSHEETPAGE hPage;

  pageProps.dwSize      = sizeof(pageProps);
  pageProps.dwFlags     = PSP_DEFAULT | PSP_USEICONID | PSP_USEREFPARENT | PSP_USETITLE;
  pageProps.hInstance   = _Module.GetModuleInstance();
  pageProps.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE);
  pageProps.pszIcon     = MAKEINTRESOURCE(IDI_ICON);
  pageProps.pszTitle    = _T("VDMSound");
  pageProps.pfnDlgProc  = PropPageDlgProc;
  pageProps.lParam      = (LPARAM)NULL;
  pageProps.pfnCallback = NULL;
  pageProps.pcRefParent = (UINT*)(&(_Module.m_nLockCnt));

  // Create the property sheet
  hPage = CreatePropertySheetPage(&pageProps);

  if (!hPage)
    return HRESULT_FROM_WIN32(GetLastError());

  // Call the shell's callback function, so it adds the page to
  // the property sheet
  if (!lpfnAddPageProc(hPage, lParam)) {
    DestroyPropertySheetPage(hPage);
    return E_UNEXPECTED;
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK PropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG:
      return OnInitDialog(hWnd, lParam);

    case WM_NOTIFY:
      if (lParam) {
        NMHDR* pHdr = (NMHDR*)lParam;

        switch (pHdr->code) {
          case PSN_APPLY:
            return OnApply(hWnd, (PSHNOTIFY*)pHdr);
        }
      } break;
  }

  return FALSE;
}

BOOL OnInitDialog(HWND hWnd, LPARAM lParam) {
  PROPSHEETPAGE* pPSP = (PROPSHEETPAGE*)(lParam);

  if (pPSP) {
    // TODO: implement

  }

  return FALSE;
}

BOOL OnApply(HWND hWnd, PSHNOTIFY* pHdr) {
  if (pHdr) {
    // TODO: implement

    // Return PSNRET_NOERROR to allow the sheet to close if the user clicked OK.
    SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);
    return TRUE;
  } else {
    return FALSE;
  }
}
