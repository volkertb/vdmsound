// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__31FAAD0B_55F2_4B90_B9CF_C878A0F235ED__INCLUDED_)
#define AFX_STDAFX_H__31FAAD0B_55F2_4B90_B9CF_C878A0F235ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

#ifdef _M_IX86
#define i386
#endif

#ifdef __cplusplus
extern "C" {
#endif
# include <nt_vdd.h>
# include <vddsvc.h>
#ifdef __cplusplus
}
#endif

#pragma comment ( lib , "ntvdm.lib" )

#include <mmsystem.h>
#pragma comment ( lib , "winmm.lib" )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__31FAAD0B_55F2_4B90_B9CF_C878A0F235ED__INCLUDED_)
