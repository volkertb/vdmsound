#ifndef __LAUNCHPADSETTINGS_H
#define __LAUNCHPADSETTINGS_H

class CLaunchPadSettings {
  public:
	  CLaunchPadSettings(LPCTSTR fileName);
	  CLaunchPadSettings(const CStringArray& fileNames);
	  CLaunchPadSettings(const CLaunchPadSettings& settings);
	  virtual ~CLaunchPadSettings(void);

  public:
    VOID GetFileNames(CStringArray& fileNames) const;
    CString GetFileNames(LPCTSTR szSeparators = _T("\n"), int itemLimit = 0) const;

    HRESULT GetValue(LPCTSTR section, LPCTSTR key, CString& value, BOOL* isIndeterminate = NULL, LPCTSTR defValue = _T(""));
    HRESULT SetValue(LPCTSTR section, LPCTSTR key, LPCTSTR value, BOOL doWriteThrough = FALSE);
    HRESULT UnsetValue(LPCTSTR section, LPCTSTR key);

    HRESULT MakeDirty(LPCTSTR section, LPCTSTR key);
    HRESULT MakeDirty(void);

    HRESULT Commit(LPCTSTR section, LPCTSTR key);
    HRESULT Commit(void);

    BOOL IsDirty(LPCTSTR section, LPCTSTR key) const;
    BOOL IsDirty(void) const;

    HRESULT LoadFromTemplate(LPCTSTR templateFile);
    HRESULT LoadFromTemplate(const CLaunchPadSettings& templateSettings);

    void Copy(const CLaunchPadSettings& src);
    void Reset(void);

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

    typedef CMap<SettingKey,SettingKey&,SettingValue,SettingValue&> CSettingsMap;

  // Utility
  protected:
    static BOOL GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, CString& result, LPCTSTR lpFileName);
    static BOOL WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName);
    static BOOL GetPrivateProfileSectionNames(CStringArray& result, LPCTSTR lpFileName);
    static BOOL GetPrivateProfileSection(LPCTSTR lpAppName, CStringArray& result, LPCTSTR lpFileName);

    static BOOL GetAllStrings(const CStringArray& fileNames, CSettingsMap& settingsCache);

    static VOID TranslateTo(LPCTSTR plain, CString& translated);
    static VOID TranslateFrom(LPCTSTR translated, CString& plain);
    static BOOL NeedsTranslation(TCHAR tch);

  // Member variables
  protected:
    CStringArray m_fileNames;
    CSettingsMap m_settingsCache;
};

#endif // __LAUNCHPADSETTINGS_H
