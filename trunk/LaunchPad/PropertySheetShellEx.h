// PropertySheetShellEx.h : Declaration of the CPropertySheetShellEx

#ifndef __PROPERTYSHEETSHELLEX_H_
#define __PROPERTYSHEETSHELLEX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#include <comdef.h>
#include <shlobj.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetShellEx
class ATL_NO_VTABLE CPropertySheetShellEx : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPropertySheetShellEx, &CLSID_PropertySheetShellEx>,
	public IShellExtInit
{
public:
	CPropertySheetShellEx()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PROPERTYSHEETSHELLEX)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPropertySheetShellEx)
	COM_INTERFACE_ENTRY(IShellExtInit)
END_COM_MAP()

// IShellExtInit
public:
  STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);
};

#endif //__PROPERTYSHEETSHELLEX_H_
