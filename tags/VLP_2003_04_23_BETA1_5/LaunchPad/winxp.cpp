#include "stdafx.h"
#include "resource.h"

#include "winxp.h"

#ifndef _UNICODE
#define SZFNCREATEACTCTX     "CreateActCtxA"
#define SZFNACTIVATEACTCTX   "ActivateActCtxA"
#define SZFNDEACTIVATEACTCTX "DeactivateActCtxA"
#define SZFNRELEASEACTCTX    "ReleaseActCtxA"
#else
#define SZFNCREATEACTCTX     "CreateActCtxW"
#define SZFNACTIVATEACTCTX   "ActivateActCtxW"
#define SZFNDEACTIVATEACTCTX "DeactivateActCtxW"
#define SZFNRELEASEACTCTX    "ReleaseActCtxW"
#endif // _UNICODE

CWinXPActivationContext::CWinXPActivationContext(void)
: m_bInit(FALSE), m_bActive(FALSE),
  m_pCreateActCtx(NULL), m_pActivateActCtx(NULL), m_pDeactivateActCtx(NULL), m_pReleaseActCtx(NULL),
  m_ulActivationCookie(0), m_hActCtx(0)
{
}

CWinXPActivationContext::~CWinXPActivationContext(void) {
  if (m_bActive)
    Deactivate();

  if (m_bInit && (m_pReleaseActCtx != NULL))
    (*m_pReleaseActCtx)(m_hActCtx);
}

BOOL CWinXPActivationContext::Init(void) {
  if (m_bInit)
    return TRUE;

  if (!InitAPI())
    return FALSE;

  OSVERSIONINFO info;
  memset(&info, 0, sizeof(info));

  info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  
  if (GetVersionEx(&info)) {
    //do special XP theme activation code only on XP or higher...
    if ((m_pCreateActCtx != NULL) &&
        (info.dwMajorVersion >= 5) &&
        (info.dwMinorVersion >= 1) &&
        (info.dwPlatformId == VER_PLATFORM_WIN32_NT))
    {
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

      m_hActCtx = (*m_pCreateActCtx)(&actctx);

      if (m_hActCtx != INVALID_HANDLE_VALUE) {
        m_bInit = TRUE;
      } else {
        TRACE(_T("LaunchPad: CWinXPActivationContext::Init() - Obtained invalid handle, CreateActCtx() failed (0x%08x)\n"), GetLastError());
      }

      return m_bInit;
    } else {
      TRACE(_T("LaunchPad: CWinXPActivationContext::Init() - Not running WinXP (has %d.%d, needs 5.1+)\n"), (int)info.dwMajorVersion, (int)info.dwMinorVersion);
    }
  }

  return FALSE;
}

BOOL CWinXPActivationContext::Activate(void) {
  if (!m_bInit || (m_pActivateActCtx == NULL))
    return FALSE;

  if ((*m_pActivateActCtx)(m_hActCtx, &m_ulActivationCookie)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Activate() - Activation context activated\n"));
    m_bActive = TRUE;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Activate() - Activation failed (0x%08x)\n"), GetLastError());
  }

  return m_bActive;
}

BOOL CWinXPActivationContext::Deactivate(void) {
  if (!m_bInit || !m_bActive || (m_pDeactivateActCtx == NULL))
    return FALSE;

  if ((*m_pDeactivateActCtx)(0, m_ulActivationCookie)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Activate() - Activation context deactivated\n"));
    m_bActive = FALSE;
  } else {
    TRACE(_T("LaunchPad: CWinXPActivationContext::Deactivate() - Deactivation failed (0x%08x)\n"), GetLastError());
  }

  return !m_bActive;
}

BOOL CWinXPActivationContext::InitAPI(void) {
  HMODULE hKernel32   = GetModuleHandle(_T("KERNEL32"));
  m_pCreateActCtx     = (LPFNCREATEACTCTX)    GetProcAddress(hKernel32, SZFNCREATEACTCTX);
  m_pActivateActCtx   = (LPFNACTIVATEACTCTX)  GetProcAddress(hKernel32, SZFNACTIVATEACTCTX);
  m_pDeactivateActCtx = (LPFNDEACTIVATEACTCTX)GetProcAddress(hKernel32, SZFNDEACTIVATEACTCTX);
  m_pReleaseActCtx    = (LPFNRELEASEACTCTX)   GetProcAddress(hKernel32, SZFNRELEASEACTCTX);

  if ((m_pCreateActCtx == NULL) || (m_pActivateActCtx == NULL) || (m_pDeactivateActCtx == NULL) || (m_pReleaseActCtx == NULL)) {
    TRACE(_T("LaunchPad: CWinXPActivationContext::InitAPI() - At least one xxxActCtx() method was not found\n"));
    return FALSE;
  } else {
    return TRUE;
  }
}
