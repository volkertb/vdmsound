#ifndef __LAUNCHPADUTIL_H
#define __LAUNCHPADUTIL_H

class CLaunchPadSettings;

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
// VLPUtil
//////////////////////////////////////////////////////////////////////

namespace VLPUtil {
  // Constants
  extern LPCTSTR T_YES;
  extern LPCTSTR T_NO;

  // GUI helper functions
  HRESULT SyncCheckBox(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState = FALSE, LPCTSTR yesValue = T_YES, LPCTSTR noValue = T_NO);
  HRESULT SyncRadioButton(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState, LPCTSTR selValue);
  HRESULT SyncRadioButton(BOOL bSave, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, LPCTSTR selValue1, ...);

  HRESULT LoadIconCtl(CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CStatic_Icon& control);

  // GDI helper functions
  HRESULT LoadDIBFromIcon(CDIBitmap& bmp, HINSTANCE hInstance, LPCTSTR lpIconName, HBRUSH hBgBrush, UINT cx = 0, UINT cy = 0, BOOL stretch = TRUE, UINT cx1 = 0, UINT cy1 = 0);

  // Shell helper functions
  void ParseIconLocation(LPCTSTR iconLocation, CString& iconPath, int& iconIndex);
  CString GetRelativePath(LPCTSTR filePath, BOOL isPathOnly, LPCTSTR basePath);
  CString GetAbsolutePath(LPCTSTR filePath, BOOL isPathOnly, LPCTSTR basePath);
  CString GetDirectory(LPCTSTR filePath);

  // Other shell helper functions
  BOOL isVLPFile(LPCTSTR fName);
  BOOL isMSDOSFile(LPCTSTR fName);
};

#endif // __LAUNCHPADUTIL_H
