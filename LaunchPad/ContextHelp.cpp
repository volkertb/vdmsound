// ContextHelp.cpp: implementation of the CContextHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ContextHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////

CCriticalSection CContextHelp::ownershipMapLock;
CMap<HWND,HWND,CContextHelp*,CContextHelp*> CContextHelp::ownershipMap;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextHelp::CContextHelp() : m_pWnd(NULL), m_pOldWndProc(NULL)
{
}

CContextHelp::~CContextHelp()
{
}


BOOL CContextHelp::Attach(CWnd* pWnd) {
  ASSERT_VALID(pWnd);
  ASSERT(m_pWnd == NULL);
  ASSERT(m_pOldWndProc == NULL);

  if (pWnd == NULL)
    return FALSE;

  CSingleLock lock(&ownershipMapLock, TRUE);

#ifdef _DEBUG
  CContextHelp* pTmp;
#endif

  ASSERT(!ownershipMap.Lookup(pWnd->m_hWnd, pTmp));

  ownershipMap.SetAt(pWnd->m_hWnd, this);

  m_pWnd = pWnd;
  m_pOldWndProc = (WNDPROC)::SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LPARAM)ContextHelpWndProc);

  ASSERT(m_pOldWndProc != NULL);
  ASSERT(m_pOldWndProc != ContextHelpWndProc);

  if (m_pOldWndProc == NULL) {
    ownershipMap.RemoveKey(pWnd->m_hWnd);
    m_pWnd = NULL;
    return FALSE;
  }

  SetWindowLong(pWnd->m_hWnd, GWL_EXSTYLE, GetWindowLong(pWnd->m_hWnd, GWL_EXSTYLE) | WS_EX_CONTEXTHELP);

  return TRUE;
}

BOOL CContextHelp::Detach(void) {
  ASSERT_VALID(m_pWnd);
  ASSERT(m_pOldWndProc != NULL);
  
  if (m_pWnd == NULL)
    return FALSE;

  CSingleLock lock(&ownershipMapLock, TRUE);

  if (ownershipMap.RemoveKey(m_pWnd->m_hWnd))
    return FALSE;

  VERIFY((WNDPROC)::SetWindowLong(m_pWnd->m_hWnd, GWL_WNDPROC, (LPARAM)m_pOldWndProc) == ContextHelpWndProc);

  m_pWnd = NULL;
  m_pOldWndProc = NULL;

  return TRUE;
}



//////////////////////////////////////////////////////////////////////
// Utility methods
//////////////////////////////////////////////////////////////////////

BOOL CContextHelp::BuildWinHelpMap(HWND hWndParent, CDWordArray& winHelpMap) {
  try {
    for (HWND hWndChild = GetTopWindow(hWndParent); hWndChild != NULL; hWndChild = GetNextWindow(hWndChild, GW_HWNDNEXT)) {
      int ctlId;
      if ((ctlId = GetDlgCtrlID(hWndChild)) != -1) {
        winHelpMap.Add((DWORD)ctlId);
        winHelpMap.Add((DWORD)ctlId);
      }
    }

    winHelpMap.Add((DWORD)0);
    winHelpMap.Add((DWORD)0);

    return TRUE;
  } catch (CMemoryException* /*cme*/) {
    return FALSE;
  }
}



//////////////////////////////////////////////////////////////////////
// WM_xxx message handlers
//////////////////////////////////////////////////////////////////////

void CContextHelp::OnContextMenu(CWnd* pWnd, CPoint point) {
  CWinApp* pApp = AfxGetApp();

  ASSERT(pApp != NULL);
  ASSERT_VALID(pWnd);

  if ((pApp == NULL) || (pWnd == NULL))
    return;

  CDWordArray winHelpMap;

  if (BuildWinHelpMap(pWnd->m_hWnd, winHelpMap)) {
    WinHelp(pWnd->m_hWnd, pApp->m_pszHelpFilePath, HELP_CONTEXTMENU, (DWORD)(winHelpMap.GetData()));
  }
}

BOOL CContextHelp::OnHelpInfo(HELPINFO* pHelpInfo) {
  CWinApp* pApp = AfxGetApp();

  ASSERT(pApp != NULL);
  ASSERT(pHelpInfo != NULL);
  ASSERT(pHelpInfo->cbSize >= sizeof(HELPINFO));

  HWND hWndChild = (HWND)(pHelpInfo->hItemHandle);
  ASSERT(IsWindow(hWndChild));

  HWND hWndParent = GetParent((HWND)(pHelpInfo->hItemHandle));
  ASSERT(IsWindow(hWndParent));

  if ((pApp == NULL) || (hWndChild == NULL) || (hWndParent == NULL))
    return FALSE;

  CDWordArray winHelpMap;

  if (BuildWinHelpMap(hWndParent, winHelpMap)) {
    WinHelp(hWndChild, pApp->m_pszHelpFilePath, HELP_WM_HELP, (DWORD)(winHelpMap.GetData()));
    return TRUE;
  } else {
    return FALSE;
  }
}



//////////////////////////////////////////////////////////////////////
// Callback
//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK CContextHelp::ContextHelpWndProc(
  HWND hWnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam)  // second message parameter
{
  try {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CContextHelp* pOwner = NULL;
    CSingleLock lock(&ownershipMapLock, TRUE);
    ownershipMap.Lookup(hWnd, pOwner);
    lock.Unlock();

    if (pOwner == NULL)
      return DefWindowProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
      case WM_CONTEXTMENU:
        pOwner->OnContextMenu(CWnd::FromHandle(hWnd), CPoint(LOWORD(lParam), HIWORD(lParam)));
        return 0;

      case WM_HELP:
        return pOwner->OnHelpInfo((HELPINFO*)lParam);
    }

    return (pOwner->m_pOldWndProc)(hWnd, uMsg, wParam, lParam);
  } catch (...) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}
