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
// WM_xxx message handlers
//////////////////////////////////////////////////////////////////////

void CContextHelp::OnContextMenu(CWnd* pWnd, CPoint point) {
  CPoint clientPoint(point);
  HWND hHitWnd = WindowFromPoint(point);
  ScreenToClient(hHitWnd, &clientPoint);
  m_ctxHlpWnd   = ChildWindowFromPointEx(hHitWnd, clientPoint, CWP_ALL);
  m_ctxHlpPoint = point;

  ASSERT(IsWindow(m_ctxHlpWnd));

  if ((m_ctxHlpWnd == NULL) || (m_pWnd->m_hWnd == m_ctxHlpWnd))
    return;

  CMenu menu;

  if (!menu.LoadMenu(IDR_MNU_WHATSTHIS))
    return;

  CMenu* pPopup = menu.GetSubMenu(0);

  ASSERT(pPopup != NULL);

  if (pPopup == NULL)
    return;

  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_pWnd);
}

void CContextHelp::OnWhatsthis() {
  ASSERT(IsWindow(m_ctxHlpWnd));

  if (m_ctxHlpWnd == NULL)
    return;

  HELPINFO helpInfo;
  helpInfo.cbSize       = sizeof(helpInfo);
  helpInfo.iContextType = HELPINFO_WINDOW;
  helpInfo.iCtrlId      = GetDlgCtrlID(m_ctxHlpWnd);
  helpInfo.hItemHandle  = m_ctxHlpWnd;
  helpInfo.dwContextId  = 0;
  helpInfo.MousePos     = m_ctxHlpPoint;

  SendMessage(m_pWnd->m_hWnd, WM_HELP, (WPARAM)0, (LPARAM)(&helpInfo));
}

BOOL CContextHelp::OnHelpInfo(HELPINFO* pHelpInfo) {
  CWinApp* pApp = AfxGetApp();

  ASSERT(pApp != NULL);
  ASSERT(pHelpInfo != NULL);
  ASSERT(pHelpInfo->cbSize >= sizeof(HELPINFO));

  HELPINFO helpInfo;
  memset(&helpInfo, 0, sizeof(helpInfo));
  memcpy(&helpInfo, pHelpInfo, min(sizeof(helpInfo), pHelpInfo->cbSize));

  if (helpInfo.iContextType == HELPINFO_WINDOW) {
    // Obtain the coordinates of the relevant control in order to position
    //  the help popup window
    RECT wndRect = { helpInfo.MousePos.x, helpInfo.MousePos.y,
                     helpInfo.MousePos.x, helpInfo.MousePos.y };
    ::GetWindowRect((HWND)(pHelpInfo->hItemHandle), &wndRect);

    // Static controls don't have help associated with them, so we try to locate the next
    //  non-static control in order to obtain its help description
    for (int retry = 0; (helpInfo.iCtrlId == IDC_STATIC) && (retry < 100); retry++) {
      helpInfo.hItemHandle = GetNextDlgTabItem(m_pWnd->m_hWnd, (HWND)(helpInfo.hItemHandle), FALSE);
      helpInfo.iCtrlId = GetDlgCtrlID((HWND)(helpInfo.hItemHandle));
    }

    // Position and show popup help window
    pApp->WinHelp(MAKELONG((wndRect.left + wndRect.left) / 2, (wndRect.top + wndRect.bottom) / 2), HELP_SETPOPUP_POS);
    pApp->WinHelp(helpInfo.iCtrlId, HELP_CONTEXTPOPUP);
  }

  return TRUE;
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

      case WM_COMMAND:
        if ((HIWORD(wParam) == 0) && (LOWORD(wParam) == ID_WHATSTHIS)) {
          pOwner->OnWhatsthis();
          return 0;
        } break;

      case WM_HELP:
        return pOwner->OnHelpInfo((HELPINFO*)lParam);
    }

    return (pOwner->m_pOldWndProc)(hWnd, uMsg, wParam, lParam);
  } catch (...) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}
