#include "stdafx.h"
#include "resource.h"

#include "winxp.h"

#ifndef _UNICODE
#define SZFNCREATEACTCTX     "CreateActCtxA"
#else
#define SZFNCREATEACTCTX     "CreateActCtxW"
#endif // _UNICODE
#define SZFNACTIVATEACTCTX   "ActivateActCtx"
#define SZFNDEACTIVATEACTCTX "DeactivateActCtx"
#define SZFNRELEASEACTCTX    "ReleaseActCtx"
#define SZFNGETCURRENTACTCTX "GetCurrentActCtx"

CWinXPActivationContext::CWinXPActivationContext(void)
: m_ulActivationCookie(0), m_hActCtx(INVALID_HANDLE_VALUE)
{
}

CWinXPActivationContext::~CWinXPActivationContext(void) {
  Deactivate();
  Release();
}

BOOL CWinXPActivationContext::Create(void) {
  if (m_hActCtx != INVALID_HANDLE_VALUE)
    return TRUE;

  LPFNCREATEACTCTX pCreateActCtx = (LPFNCREATEACTCTX)GetKERNEL32ProcAddress(SZFNCREATEACTCTX);

  if (pCreateActCtx == NULL)
    return FALSE;

  ACTCTX actctx;
  memset(&actctx, 0, sizeof(actctx));

  TCHAR szModule[MAX_PATH] = { 0 };

  HINSTANCE hInst = AfxGetInstanceHandle();
  ::GetModuleFileName(hInst, szModule, MAX_PATH);

  actctx.cbSize = sizeof(ACTCTX);
  actctx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

  actctx.lpSource = szModule;
  actctx.lpResourceName = ISOLATIONAWARE_MANIFEST_RESOURCE_ID;
  actctx.hModule = hInst;

  m_hActCtx = (*pCreateActCtx)(&actctx);

  if (m_hActCtx != INVALID_HANDLE_VALUE) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Create() - Activation context created\n"));
    return TRUE;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Create() - Obtained invalid handle, CreateActCtx() failed (0x%08x)\n"), GetLastError());
    return FALSE;
  }
}

BOOL CWinXPActivationContext::Activate(void) {
  if (m_hActCtx == INVALID_HANDLE_VALUE)
    return FALSE;

  LPFNACTIVATEACTCTX pActivateActCtx = (LPFNACTIVATEACTCTX)GetKERNEL32ProcAddress(SZFNACTIVATEACTCTX);

  if (pActivateActCtx == NULL)
    return FALSE;

  if ((*pActivateActCtx)(m_hActCtx, &m_ulActivationCookie)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Activate() - Activation context activated\n"));
    return TRUE;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Activate() - Activation failed (0x%08x)\n"), GetLastError());
    return FALSE;
  }
}

BOOL CWinXPActivationContext::Deactivate(void) {
  if (m_hActCtx == INVALID_HANDLE_VALUE)
    return FALSE;

  LPFNDEACTIVATEACTCTX pDeactivateActCtx = (LPFNDEACTIVATEACTCTX)GetKERNEL32ProcAddress(SZFNDEACTIVATEACTCTX);

  if (pDeactivateActCtx == NULL)
    return FALSE;

  if ((*pDeactivateActCtx)(0, m_ulActivationCookie)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Deactivate() - Activation context deactivated\n"));
    return TRUE;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Deactivate() - Deactivation failed (0x%08x)\n"), GetLastError());
    return FALSE;
  }
}

BOOL CWinXPActivationContext::Release(void) {
  if (m_hActCtx == INVALID_HANDLE_VALUE)
    return FALSE;

  LPFNRELEASEACTCTX pReleaseActCtx = (LPFNRELEASEACTCTX)GetKERNEL32ProcAddress(SZFNRELEASEACTCTX);

  if (pReleaseActCtx == NULL)
    return FALSE;

  (*pReleaseActCtx)(m_hActCtx);

  m_hActCtx = INVALID_HANDLE_VALUE;

  TRACE(_T("LaunchPad: CWinXPActivationContext::Release() - Activation context released\n"));
  return TRUE;
}

HANDLE CWinXPActivationContext::GetCurrentContext(void) {
  LPFNGETCURRENTACTCTX pGetCurrentActCtx = (LPFNGETCURRENTACTCTX)GetKERNEL32ProcAddress(SZFNGETCURRENTACTCTX);

  if (pGetCurrentActCtx == NULL)
    return NULL;

  HANDLE hActCtx = NULL;

  if ((*pGetCurrentActCtx)(&hActCtx)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::GetCurrentContext() - Current activation context obtained\n"));
    return hActCtx;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::GetCurrentContext() - Retrieval failed (0x%08x)\n"), GetLastError());
    return NULL;
  }
}



/////////////////////////////////////////////////////////////////////////////



FARPROC CWinXPActivationContext::GetKERNEL32ProcAddress(LPCSTR lpszMethod) {
  HMODULE hKernel32 = GetModuleHandle(_T("KERNEL32"));
  return hKernel32 ? GetProcAddress(hKernel32, lpszMethod) : NULL;
}
