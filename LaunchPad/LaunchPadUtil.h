#ifndef __LAUNCHPADUTIL_H
#define __LAUNCHPADUTIL_H

class CLaunchPadSettings;

//////////////////////////////////////////////////////////////////////

#include <aclapi.h>

//////////////////////////////////////////////////////////////////////
// CStatic_Icon
//////////////////////////////////////////////////////////////////////

class CStatic_Icon : public CStatic {
  public:
    CStatic_Icon(void);
    virtual ~CStatic_Icon(void);

  public:
    HRESULT LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName);
    HRESULT ExtractIcon(HINSTANCE hInstance, LPCTSTR lpszExeFileName, UINT nIconIndex);
    BOOL DeleteIcon(void);

  public:
    HICON m_hIcon;

  protected:
    BOOL m_bAttached;
    UINT m_uType;
};



//////////////////////////////////////////////////////////////////////
// CDIBitmap
//////////////////////////////////////////////////////////////////////

class CDIBitmap : public CBitmap {
  public:
    CDIBitmap(void);

  public:
    BOOL CreateDIBitmap(CDC* pdc, const LPBITMAPINFOHEADER lpbmih, LPCVOID lpbInit = NULL, const LPBITMAPINFO lpbmi = NULL, UINT fuUsage = DIB_RGB_COLORS);
    BOOL CreateDIBitmap(int nWidth, int nHeight, UINT nBitcount = 24, int numColors = 0);
    UINT SetDIBColorTable(CDC* pdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors);
    UINT SetDIBits(CDC* pdc, UINT uStartScan, UINT cScanLines, CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi, UINT fuColorUse);
};



//////////////////////////////////////////////////////////////////////
// COpenDOSProgramDialog
//////////////////////////////////////////////////////////////////////

class COpenDOSProgramDialog : public CFileDialog {
  public:
    COpenDOSProgramDialog(LPCTSTR lpszFileName = NULL, CWnd* pParentWnd = NULL);

  protected:
    virtual BOOL OnFileNameOK(void);

  protected:
    CString m_strFilter;
};



//////////////////////////////////////////////////////////////////////
// CDDEdit
//////////////////////////////////////////////////////////////////////

class CDDEdit : public CEdit {
  public:
    CDDEdit(void);

  public:
  	int Register(void);

  protected:
    class COleDropTarget : public ::COleDropTarget {
      DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
      DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
      BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    };

  protected:
    COleDropTarget m_dropTarget;
};



//////////////////////////////////////////////////////////////////////
// VLPUtil
//////////////////////////////////////////////////////////////////////

class CLaunchPadSettings;

namespace VLPUtil {
  // Constants
  extern LPCTSTR T_YES;
  extern LPCTSTR T_NO;

  // GUI helper functions
  HRESULT SyncCheckBox(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState = FALSE, LPCTSTR yesValue = T_YES, LPCTSTR noValue = T_NO);
  HRESULT SyncRadioButton(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState, LPCTSTR selValue);
  HRESULT SyncRadioButton(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, CString& otherValueStorage, LPCTSTR selValue1, ...);
  HRESULT SyncEditBox(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CWnd& control, LPCTSTR defValue = _T(""));
  HRESULT SyncComboBox(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CComboBox& control, LPCTSTR defValue);
  HRESULT SyncGroup(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState = FALSE, LPCTSTR yesValue = T_YES, LPCTSTR noValue = T_NO);

  HRESULT LoadIconCtl(CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CStatic_Icon& control, LPCTSTR defValue = _T(""));

  // GDI helper functions
  HRESULT LoadDIBFromIcon(CDIBitmap& bmp, HINSTANCE hInstance, LPCTSTR lpIconName, HBRUSH hBgBrush, UINT cx = 0, UINT cy = 0, BOOL stretch = TRUE, UINT cx1 = 0, UINT cy1 = 0);

  // Shell helper functions
  void ParseIconLocation(LPCTSTR iconLocation, CString& iconPath, int& iconIndex);
  CString GetRelativePath(LPCTSTR filePath, LPCTSTR baseDir, BOOL isPathOnly = FALSE);
  CString GetAbsolutePath(LPCTSTR filePath, LPCTSTR baseDir, BOOL isPathOnly = FALSE);
  CString GetDirectory(LPCTSTR filePath);
  CString GetFilename(LPCTSTR filePath, BOOL removeExtension = FALSE);
  BOOL IsDirectory(LPCTSTR pszPath);
  BOOL IsRelativePath(LPCTSTR pszPath);
  CString RenameExtension(LPCTSTR filePath, LPCTSTR ext);
  BOOL FileExists(LPCTSTR filePath);
  HRESULT GetSpecialFolderPath(CString& result, int nFolder, HWND hwndOwner = NULL);

  CString GetExpandedPath(LPCTSTR filePath);
  CString GetShortPath(LPCTSTR fileLongPath);

  HRESULT EnableAutoComplete(HWND hwndEdit);

  BOOL BrowseForFile(CString& fileName, BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
  BOOL BrowseForFolder(CString& folderName, UINT ulFlags = BIF_RETURNONLYFSDIRS, LPCTSTR lpszTitle = NULL, LPCITEMIDLIST pidlRoot = NULL, CWnd* pParentWnd = NULL);

  // Other shell helper functions
  BOOL isVLPFile(LPCTSTR fName);
  BOOL isMSDOSFile(LPCTSTR fName);
  CString GetVDMSFilePath(LPCTSTR fileName);

  // NT security helper functions
  HRESULT GetEffectiveRights(LPCTSTR pObjectName, SE_OBJECT_TYPE ObjectType, PACCESS_MASK pAccessRights);

  // Message and string formatting helper functions
  CString AFX_CDECL FormatMessage(DWORD dwMessageID, bool bFromSystem = true, HMODULE hModule = NULL, DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), bool bIgnoreInserts = true, ...);
  CString AFX_CDECL FormatString(LPCTSTR lpszFormat, ...);

  CString LoadString(UINT nID);

  // COM helper class
  // Keeps COM initialized during the lifetime of the object
  class CComKeepAlive {
    public:
      CComKeepAlive(void) : m_hr(E_FAIL)
        { m_hr = CoInitialize(NULL); }
      ~CComKeepAlive(void)
        { if (SUCCEEDED(m_hr)) CoUninitialize(); }
    protected:
      HRESULT m_hr;
  };
};

#endif // __LAUNCHPADUTIL_H
