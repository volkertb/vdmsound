// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__9CA8183E_C3D3_463A_9E43_D178D65751DD__INCLUDED_)
#define AFX_STDAFX_H__9CA8183E_C3D3_463A_9E43_D178D65751DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif // _WIN32_WINNT

#ifndef _WIN32_IE 
#define _WIN32_IE 0x0400
#endif // _WIN32_IE

#ifndef ISOLATION_AWARE_ENABLED
#define ISOLATION_AWARE_ENABLED 1
#endif

#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxole.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Reference additional headers your program requires here

#include <afxdlgs.h>
#include <afxtempl.h>
#include <afxmt.h>
#include <afxcmn.h>

#include <comdef.h>
#include <shlobj.h>
#pragma comment ( lib , "shlwapi.lib" )

#include <mmsystem.h>
#pragma comment ( lib , "winmm.lib" )

#include <dsound.h>
#pragma comment ( lib , "dsound.lib" )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9CA8183E_C3D3_463A_9E43_D178D65751DD__INCLUDED)
