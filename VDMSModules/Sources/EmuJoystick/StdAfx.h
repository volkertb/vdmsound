// stdafx.h : Include-Datei f�r Standard-System-Include-Dateien,
//      oder h�ufig verwendete, projektspezifische Include-Dateien,
//      die nur in unregelm��igen Abst�nden ge�ndert werden.

#if !defined(AFX_STDAFX_H__756E23B4_57C2_4ED0_8D23_D7871712D193__INCLUDED_)
#define AFX_STDAFX_H__756E23B4_57C2_4ED0_8D23_D7871712D193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//Sie k�nnen eine Klasse von CComModule ableiten und diese verwenden, um etwas zu �berschreiben,
//Sie sollten jedoch den Namen von _Module nicht �ndern
extern CComModule _Module;
#include <atlcom.h>

//

#include <afxmt.h>

#include <mmsystem.h>
#pragma comment ( lib , "winmm.lib" )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__756E23B4_57C2_4ED0_8D23_D7871712D193__INCLUDED)
