// DragNDropFSM.cpp: implementation of the CDragNDropFSM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "DragNDropFSM.h"

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "RUNWITHVDMSThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDragNDropFSM::CDragNDropFSM() : m_state(STATE_IDLE)
{
}

CDragNDropFSM::~CDragNDropFSM()
{
}


BOOL CDragNDropFSM::DragEnter(DWORD grfKeyState, LPDWORD pdwEffect)
{
  return DragGeneric(grfKeyState, pdwEffect);
}

BOOL CDragNDropFSM::DragOver(DWORD grfKeyState, LPDWORD pdwEffect)
{
  if (m_state != STATE_IDLE) {
    return DragGeneric(grfKeyState, pdwEffect);
  } else {
    *pdwEffect = DROPEFFECT_NONE;   // do nothing
    return TRUE;
  }
}

BOOL CDragNDropFSM::DragLeave(VOID)
{
  m_state = STATE_IDLE;
  return TRUE;
}

BOOL CDragNDropFSM::Drop(DWORD grfKeyState, LPDWORD pdwEffect, POINTL pt, LPCTSTR target, LPCTSTR dropped)
{
  if (m_state != STATE_IDLE) {
    *pdwEffect = DROPEFFECT_MOVE;   // do nothing

    switch (m_state) {
      case STATE_REPLACE:
        return DragNReplace(target, dropped);
      case STATE_RUNWITH:
        return DragNRunWith(target, dropped);
      case STATE_POPUP:
        return DragNPopup(pt, target, dropped);
      default:
        return FALSE;
    }
  } else {
    *pdwEffect = DROPEFFECT_NONE;   // do nothing
    return TRUE;
  }
}

BOOL CDragNDropFSM::DragGeneric(DWORD grfKeyState, LPDWORD pdwEffect)
{
  if (((grfKeyState & MK_ALT) == MK_ALT) || ((grfKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT))) {
    m_state = STATE_REPLACE;
    *pdwEffect = DROPEFFECT_LINK;   // to replace target's program name with the dropped executable
  } else if ((grfKeyState & MK_LBUTTON) == MK_LBUTTON) {
    m_state = STATE_RUNWITH;
    *pdwEffect = DROPEFFECT_MOVE;   // to run dropped executable using target's settings
  } else if ((grfKeyState & MK_RBUTTON) == MK_RBUTTON) {
    m_state = STATE_POPUP;
    *pdwEffect = DROPEFFECT_MOVE;   // to pop up context menu
  } else {
    m_state = STATE_IDLE;
    *pdwEffect = DROPEFFECT_NONE;   // unknown key state
    return FALSE;
  }

  return TRUE;
}

BOOL CDragNDropFSM::DragNReplace(LPCTSTR target, LPCTSTR dropped) {
  CLaunchPadSettings settings(target);

  settings.SetValue(_T("program"), _T("executable"), dropped, TRUE);
  settings.SetValue(_T("program"), _T("params"), _T(""), TRUE);
  settings.SetValue(_T("program"), _T("workdir"), VLPUtil::GetDirectory(dropped), TRUE);

  return TRUE;
}

BOOL CDragNDropFSM::DragNRunWith(LPCTSTR target, LPCTSTR dropped) {
  return SUCCEEDED(CRUNWITHVDMSThread::CreateThread(target, dropped));  // create and start a thread that spawns the 16-bit process and waits for its termination
}

BOOL CDragNDropFSM::DragNPopup(POINTL pt, LPCTSTR target, LPCTSTR dropped) {
  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_MENU_DRAGNDROP));

  CMenu* pPopup = menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);

  pPopup->SetDefaultItem(ID_RUNWITH);

  DWORD cmd = (DWORD)(pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, CWnd::GetActiveWindow()));

  switch (cmd) {
    case ID_REPLACE:
      return DragNReplace(target, dropped);

    case ID_RUNWITH:
      return DragNRunWith(target, dropped);

    case IDCANCEL:
      // do nothing
      return TRUE;

    default:
      ASSERT(FALSE);
      return FALSE;
  }
}
