#ifndef __WINXP_H
#define __WINXP_H

#define XAC_MANAGE_STATE() \
  CWinXPActivationContext _xpactctx; \
  _xpactctx.Create(); \
  _xpactctx.Activate();

class CWinXPActivationContext {
  public:
    CWinXPActivationContext(void);
    ~CWinXPActivationContext(void);

    BOOL Create(void);
    BOOL Activate(void);
    BOOL Deactivate(void);
    BOOL Release(void);

    static HANDLE GetCurrentContext(void);

  private:
    typedef HANDLE (WINAPI * LPFNCREATEACTCTX)(PCACTCTX);
    typedef BOOL   (WINAPI * LPFNACTIVATEACTCTX)(HANDLE,ULONG_PTR*);
    typedef BOOL   (WINAPI * LPFNDEACTIVATEACTCTX)(DWORD,ULONG_PTR);
    typedef VOID   (WINAPI * LPFNRELEASEACTCTX)(HANDLE);
    typedef BOOL   (WINAPI * LPFNGETCURRENTACTCTX)(HANDLE*);

  private:
    static FARPROC GetKERNEL32ProcAddress(LPCSTR lpszMethod);

  private:
    ULONG_PTR m_ulActivationCookie;
    HANDLE m_hActCtx;
};

#endif // __WINXP_H
