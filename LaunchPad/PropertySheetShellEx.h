// PropertySheetShellEx.h : Declaration of the CPropertySheetShellEx

#ifndef __PROPERTYSHEETSHELLEX_H_
#define __PROPERTYSHEETSHELLEX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetShellEx
class ATL_NO_VTABLE CPropertySheetShellEx : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPropertySheetShellEx, &CLSID_PropertySheetShellEx>,
	public IShellExtInit,
  public IShellPropSheetExt
{
public:
	CPropertySheetShellEx()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PROPERTYSHEETSHELLEX)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPropertySheetShellEx)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()

// IShellExtInit
public:
  STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

// IShellPropSheetExt
public:
  STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE, LPARAM);
  STDMETHOD(ReplacePage)(UINT, LPFNADDPROPSHEETPAGE, LPARAM) { return E_NOTIMPL; }

// Member variables
protected:
  CArray<CString,LPCTSTR> m_fileNames;
};

#endif //__PROPERTYSHEETSHELLEX_H_
