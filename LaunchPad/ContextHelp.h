// ContextHelp.h: interface for the CContextHelp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTHELP_H__901E1ED8_38AB_4734_A466_37B125BA0F05__INCLUDED_)
#define AFX_CONTEXTHELP_H__901E1ED8_38AB_4734_A466_37B125BA0F05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CContextHelp
{
public:
	CContextHelp();
	virtual ~CContextHelp();

  BOOL Attach(CWnd* pWnd);
  BOOL Detach(void);

protected:
  static LRESULT CALLBACK ContextHelpWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  static CCriticalSection ownershipMapLock;
  static CMap<HWND,HWND,CContextHelp*,CContextHelp*> ownershipMap;

protected:
  void OnContextMenu(CWnd* pWnd, CPoint point);
  void OnWhatsthis();
  BOOL OnHelpInfo(HELPINFO* pHelpInfo);

protected:
  CWnd* m_pWnd;
  WNDPROC m_pOldWndProc;

  HWND  m_ctxHlpWnd;
  POINT m_ctxHlpPoint;
};

#endif // !defined(AFX_CONTEXTHELP_H__901E1ED8_38AB_4734_A466_37B125BA0F05__INCLUDED_)
