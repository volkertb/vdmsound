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
// LaunchPadSettingsHelper
//////////////////////////////////////////////////////////////////////

namespace LaunchPadSettingsHelper {
  // Constants
  extern LPCTSTR T_YES;
  extern LPCTSTR T_NO;

  // GUI helper functions
  HRESULT SyncCheckBox(BOOL isSetOp, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState = FALSE, LPCTSTR yesValue = T_YES, LPCTSTR noValue = T_NO);
  HRESULT SyncRadioButton(BOOL isSetOp, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, BOOL defState, LPCTSTR selValue);
  HRESULT SyncRadioButton(BOOL isSetOp, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CButton& control, LPCTSTR selValue1, ...);
  HRESULT SyncIcon(BOOL isSetOp, CLaunchPadSettings& settings, LPCTSTR section, LPCTSTR key, CIcon& control);

  void ParseIconString(const CString& iconString, CString& iconPath, int& iconIndex);
};

#endif // __LAUNCHPADUTIL_H
