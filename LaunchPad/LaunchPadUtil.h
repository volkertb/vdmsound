#ifndef __LAUNCHPADUTIL_H
#define __LAUNCHPADUTIL_H

class CLaunchPadSettings;

//////////////////////////////////////////////////////////////////////
// CIcon
//////////////////////////////////////////////////////////////////////

class CIcon : public CStatic {
  public:
    CIcon(void);
    virtual ~CIcon(void);

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

  HRESULT LoadIconCtl(CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CIcon& control);

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
