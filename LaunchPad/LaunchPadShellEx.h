// LaunchPadShellEx.h : Declaration of the CLaunchPadShellEx

#ifndef __LAUNCHPADSHELLEX_H_
#define __LAUNCHPADSHELLEX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLaunchPadShellEx
class ATL_NO_VTABLE CLaunchPadShellEx : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLaunchPadShellEx, &CLSID_LaunchPadShellEx>,
  public IShellExtInit,         // Common (Property sheet, Context menu)
  public IShellPropSheetExt,    // Property sheet
  public IContextMenu,          // Context menu
  public IPersistFile,          // Common (Icon, Tooltip)
  public IExtractIcon,          // Icon
  public IQueryInfo             // Tooltip
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
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IExtractIcon)
	COM_INTERFACE_ENTRY(IQueryInfo)
END_COM_MAP()

// IShellExtInit
public:
  STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

// IShellPropSheetExt
public:
  STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE, LPARAM);
  STDMETHOD(ReplacePage)(UINT, LPFNADDPROPSHEETPAGE, LPARAM)
    { return E_NOTIMPL; }

// IContextMenu
public:
  STDMETHOD(GetCommandString)(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);
  STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
  STDMETHOD(QueryContextMenu)(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

// IPersistFile
public:
  STDMETHOD(GetClassID)(CLSID* /*pClassID*/)
    { return E_NOTIMPL; }
  STDMETHOD(IsDirty)(void)
    { return E_NOTIMPL; }
  STDMETHOD(Save)(LPCOLESTR /*pszFileName*/, BOOL /*fRemember*/)
    { return E_NOTIMPL; }
  STDMETHOD(SaveCompleted)(LPCOLESTR /*pszFileName*/)
    { return E_NOTIMPL; }
  STDMETHOD(GetCurFile)(LPOLESTR* /*ppszFileName*/)
    { return E_NOTIMPL; }
  STDMETHOD(Load)(LPCOLESTR pszFileName, DWORD dwMode);

// IExtractIcon
public:
  STDMETHOD(Extract)(LPCTSTR pszFile, UINT nIconIndex, HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize);
  STDMETHOD(GetIconLocation)(UINT uFlags, LPTSTR szIconFile, UINT cchMax, LPINT piIndex, UINT* pwFlags);

// IQueryInfo
public:
  STDMETHOD(GetInfoFlags)(DWORD* /*pdwFlags*/)
    { return E_NOTIMPL; }
  STDMETHOD(GetInfoTip)(DWORD dwFlags, LPWSTR* ppwszTip);
    
// Member variables
protected:
  CBitmap m_contextMenuBmp;
  CStringArray m_fileNames;
};

#endif //__LAUNCHPADSHELLEX_H_
