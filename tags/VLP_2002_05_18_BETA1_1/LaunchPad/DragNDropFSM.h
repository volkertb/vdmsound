// DragNDropFSM.h: interface for the CDragNDropFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGNDROPFSM_H__5560F077_58EF_452E_A495_CA55DC19748B__INCLUDED_)
#define AFX_DRAGNDROPFSM_H__5560F077_58EF_452E_A495_CA55DC19748B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDragNDropFSM  
{
public:
	CDragNDropFSM();
	virtual ~CDragNDropFSM();

  BOOL DragEnter(DWORD grfKeyState, LPDWORD pdwEffect);
  BOOL DragOver(DWORD grfKeyState, LPDWORD pdwEffect);
  BOOL DragLeave(VOID);
  BOOL Drop(DWORD grfKeyState, LPDWORD pdwEffect, POINTL pt, LPCTSTR target, LPCTSTR dropped);

protected:
  BOOL DragGeneric(DWORD grfKeyState, LPDWORD pdwEffect);

  BOOL DragNReplace(LPCTSTR target, LPCTSTR dropped);
  BOOL DragNRunWith(LPCTSTR target, LPCTSTR dropped);
  BOOL DragNPopup(POINTL pt, LPCTSTR target, LPCTSTR dropped);

protected:
  enum {
    STATE_IDLE, STATE_REPLACE, STATE_RUNWITH, STATE_POPUP
  } m_state;
};

#endif // !defined(AFX_DRAGNDROPFSM_H__5560F077_58EF_452E_A495_CA55DC19748B__INCLUDED_)
