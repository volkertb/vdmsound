#ifndef __WINXP_H
#define __WINXP_H

#define XAC_MANAGE_STATE() \
  CWinXPActivationContext _xpactctx; \
  _xpactctx.Init(); \
  _xpactctx.Activate();

class CWinXPActivationContext {
  public:
    CWinXPActivationContext(void);
    ~CWinXPActivationContext(void);

    BOOL Init(void);
    BOOL Activate(void);
    BOOL Deactivate(void);

  private:
    typedef HANDLE (WINAPI * LPFNCREATEACTCTX)(PCACTCTX);
    typedef BOOL   (WINAPI * LPFNACTIVATEACTCTX)(HANDLE,ULONG_PTR*);
    typedef BOOL   (WINAPI * LPFNDEACTIVATEACTCTX)(DWORD,ULONG_PTR);
    typedef VOID   (WINAPI * LPFNRELEASEACTCTX)(HANDLE);

  private:
    BOOL InitAPI(void);

  private:
    BOOL m_bInit;
    BOOL m_bActive;

    LPFNCREATEACTCTX     m_pCreateActCtx;
    LPFNACTIVATEACTCTX   m_pActivateActCtx;
    LPFNDEACTIVATEACTCTX m_pDeactivateActCtx;
    LPFNRELEASEACTCTX    m_pReleaseActCtx;

    ULONG_PTR m_ulActivationCookie;
    HANDLE m_hActCtx;
};

#endif // __WINXP_H
