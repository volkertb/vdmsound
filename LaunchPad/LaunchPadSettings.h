#ifndef __LAUNCHPADSETTINGS_H
#define __LAUNCHPADSETTINGS_H

class CLaunchPadSettings {
  public:
	  CLaunchPadSettings(const CStringArray& fileNames);
	  virtual ~CLaunchPadSettings(void);

  public:
    VOID GetFileNames(CStringArray& fileNames) const;

    HRESULT GetValue(LPCTSTR section, LPCTSTR key, CString& value, BOOL* isIndeterminate = NULL, LPCTSTR defValue = _T(""));
    HRESULT SetValue(LPCTSTR section, LPCTSTR key, LPCTSTR value, BOOL doWriteThrough = FALSE);
    HRESULT UnsetValue(LPCTSTR section, LPCTSTR key);
    HRESULT CommitValue(LPCTSTR section, LPCTSTR key);
    HRESULT CommitAll(void);

  // Types
  protected:
    struct SettingKey {
      SettingKey(LPCTSTR section = _T(""), LPCTSTR key = _T("")) : m_section(section), m_key(key)
        { }
      SettingKey(const SettingKey& src) : m_section(src.m_section), m_key(src.m_key)
        { }
      SettingKey& operator =(const SettingKey& src)
        { m_section = src.m_section; m_key = src.m_key; return *this; }
      bool operator ==(const SettingKey& rhs) const
        { return ((m_section.CollateNoCase(rhs.m_section) == 0) && (m_key.CollateNoCase(rhs.m_key) == 0)); }
      operator DWORD(void) const
        { return m_section.GetLength() + m_key.GetLength(); }

      CString m_section;
      CString m_key;
    };

    struct SettingValue {
      SettingValue(BOOL isChanged = FALSE, BOOL isIndeterminate = FALSE, LPCTSTR value = _T("")) : m_isChanged(isChanged), m_isIndeterminate(isIndeterminate), m_value(value)
        { }
      SettingValue(const SettingValue& src) : m_isChanged(src.m_isChanged), m_isIndeterminate(src.m_isIndeterminate), m_value(src.m_value)
        { }
      SettingValue& operator =(const SettingValue& src)
        { m_isChanged = src.m_isChanged; m_isIndeterminate = src.m_isIndeterminate; m_value = src.m_value; return *this; }

      BOOL m_isChanged;
      BOOL m_isIndeterminate;
      CString m_value;
    };

  // Member variables
  protected:
    CStringArray m_fileNames;
    CMap<SettingKey,SettingKey&,SettingValue,SettingValue&> m_settingsCache;
};

#endif // __LAUNCHPADSETTINGS_H
