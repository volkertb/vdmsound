// LaunchPadShellEx.h : Declaration of the CLaunchPadShellEx

#ifndef __LAUNCHPADSHELLEX_H_
#define __LAUNCHPADSHELLEX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLaunchPadShellEx
class ATL_NO_VTABLE CLaunchPadShellEx : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLaunchPadShellEx, &CLSID_LaunchPadShellEx>,
	public IShellExtInit,
  public IShellPropSheetExt
{
public:
	CLaunchPadShellEx()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LAUNCHPADSHELLEX)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CLaunchPadShellEx)
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

#endif //__LAUNCHPADSHELLEX_H_
