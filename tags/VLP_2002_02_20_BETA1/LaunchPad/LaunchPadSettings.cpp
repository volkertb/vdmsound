#include "stdafx.h"
#include "LaunchPadSettings.h"

// MFC-specific stuff
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#define TMP_BUF_SIZE  65536
#define META_CHAR     '%'
#define COMMENT_CHAR  ';'
#define META_LEN      4

//////////////////////////////////////////////////////////////////////
//
// CLaunchPadSettings
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLaunchPadSettings::CLaunchPadSettings(LPCTSTR fileName) {
  // Keep a copy of the list of file names the settings are stored in
  m_fileNames.Add(fileName);
}

CLaunchPadSettings::CLaunchPadSettings(const CStringArray& fileNames) {
  // Keep a copy of the list of file names the settings are stored in
  m_fileNames.Copy(fileNames);
}

CLaunchPadSettings::CLaunchPadSettings(const CLaunchPadSettings& settings) {
  // Keep a copy of the list of file names the settings are stored in
  Copy(settings);
}

CLaunchPadSettings::~CLaunchPadSettings(void) {
}



//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

VOID CLaunchPadSettings::GetFileNames(
  CStringArray& fileNames) const
{
  fileNames.Copy(m_fileNames);
}

CString CLaunchPadSettings::GetFileNames(LPCTSTR szSeparators, int itemLimit) const {
  CString retVal;

  if (itemLimit < 1)
    itemLimit = m_fileNames.GetSize();

  for (int i = 0; i < m_fileNames.GetSize(); i++) {
    if (i >= itemLimit)
      break;

    retVal += m_fileNames.GetAt(i);

    if ((i + 1) < m_fileNames.GetSize()) {
      retVal += szSeparators;

      if ((i + 1) >= itemLimit) {
        retVal += _T("...");
      }
    }
  }

  return retVal;
}

//
// Obtain a setting value either from the list of configuration files
//  associated with the object at construction time.
//  The first time a value is read it is cached, and the cached value
//  is returned on all subsequent calls to GetValue(...).
//
HRESULT CLaunchPadSettings::GetValue(
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  CString& value,                                     // string to hold the returned value
  BOOL* isIndeterminate,                              // more than one value is defined across associated files
  LPCTSTR defValue)                                   // default value returned if string is not defined
{
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE, defValue);    // temporary storage for the setting (if cached)

  // Return a local (cached) copy of the value (if available),
  //  otherwise retrieve it from the file(s)
  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    CString tmpBuf;             // temporary storage for values read from file
    bool isFirstValue = true;   // whether reading values from the first file in the list or not

    // Go through all configuration files
    for (int i = 0; i < m_fileNames.GetSize(); i++) {
      if (!GetPrivateProfileString(section, key, defValue, tmpBuf, m_fileNames.GetAt(i)))
        return E_FAIL;

      if (isFirstValue) {
        isFirstValue = false;
        cacheValue.m_value = tmpBuf;
        cacheValue.m_isIndeterminate = FALSE;
      } else if (tmpBuf.CollateNoCase(cacheValue.m_value) != 0) {
        cacheValue.m_value = defValue;
        cacheValue.m_isIndeterminate = TRUE;
        break;
      }
    }

    // Make a local copy of the obtained value, for future reference
    m_settingsCache.SetAt(cacheKey, cacheValue);
  }

  value = cacheValue.m_value;

  if (isIndeterminate)
    *isIndeterminate = cacheValue.m_isIndeterminate;

  return cacheValue.m_isIndeterminate ? S_FALSE : S_OK;
}

//
// Updates a setting.  Settings are not committed to the configuration file(s)
//  until Flushvalue is called.
//  If doWriteThrough is TRUE then changes are commited to disk right away.
//
HRESULT CLaunchPadSettings::SetValue(
  LPCTSTR section,                                    // ini section
  LPCTSTR key,                                        // key (string) under the given section
  LPCTSTR value,                                      // value to be stored
  BOOL doWriteThrough)                                // whether to also commit the changes to disk right away
{
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE);              // the cached setting value

  // If the setting already exists and has not been changed, bail out
  if (!doWriteThrough &&
      m_settingsCache.Lookup(cacheKey, cacheValue) &&
      !cacheValue.m_isIndeterminate &&
      (cacheValue.m_value.Collate(value) == 0))
  {
    return S_FALSE;
  }

  cacheValue.m_isChanged = TRUE;
  cacheValue.m_isIndeterminate = FALSE;
  cacheValue.m_value = value;

  m_settingsCache.SetAt(cacheKey, cacheValue);

  return doWriteThrough ? Commit(section, key) : S_OK;
}

//
// Puts given value back into an indeterminate state
//
HRESULT CLaunchPadSettings::UnsetValue(
  LPCTSTR section,
  LPCTSTR key)
{
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE);              // the cached setting value

  if (!m_settingsCache.Lookup(cacheKey, cacheValue) ||
      cacheValue.m_isIndeterminate)
  {
    return S_FALSE;
  }

  cacheValue.m_isChanged = FALSE;
  cacheValue.m_isIndeterminate = TRUE;

  m_settingsCache.SetAt(cacheKey, cacheValue);

  return S_OK;
}

//
// Marks a certain setting as dirty
//
HRESULT CLaunchPadSettings::MakeDirty(
  LPCTSTR section,                                    // ini section
  LPCTSTR key)                                        // key (string) under the given section
{
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE);              // the cached setting value

  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    ASSERT(FALSE);
    return E_INVALIDARG;
  }

  if (cacheValue.m_isIndeterminate)
    return S_FALSE;

  cacheValue.m_isChanged = TRUE;
  m_settingsCache.SetAt(cacheKey, cacheValue);

  return S_OK;
}

//
// Marks all setting as dirty
//
HRESULT CLaunchPadSettings::MakeDirty(void) {
  HRESULT hr, retVal = S_OK;
  SettingKey cacheKey;                                // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  POSITION pos = m_settingsCache.GetStartPosition();
  
  while (pos != NULL) {
    m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);

    if (FAILED(hr = MakeDirty(cacheKey.m_section, cacheKey.m_key))) {
      retVal = hr;
    }
  }

  return retVal;
}

//
// Commits changes to a certain setting to disk
//
HRESULT CLaunchPadSettings::Commit(
  LPCTSTR section,                                    // ini section
  LPCTSTR key)                                        // key (string) under the given section
{
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE);              // the cached setting value

  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    ASSERT(FALSE);
    return E_INVALIDARG;
  }

  if (!cacheValue.m_isChanged)
    return S_FALSE;

  ASSERT(!cacheValue.m_isIndeterminate);

  HRESULT hr = S_OK;

  // Go through all the files in the list of file names
  for (int i = 0; i < m_fileNames.GetSize(); i++) {
    if (!WritePrivateProfileString(section, key, cacheValue.m_value, m_fileNames.GetAt(i)))
      hr = HRESULT_FROM_WIN32(GetLastError());
  }

  // Changes were commited to disk so clear the dirty flag
  if (SUCCEEDED(hr)) {
    cacheValue.m_isChanged = FALSE;
    m_settingsCache.SetAt(cacheKey, cacheValue);
  }

  return hr;
}

//
// Commits all changes to disk
//
HRESULT CLaunchPadSettings::Commit(void) {
  HRESULT hr, retVal = S_OK;
  SettingKey cacheKey;                                // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  POSITION pos = m_settingsCache.GetStartPosition();
  
  while (pos != NULL) {
    m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);

    if (FAILED(hr = Commit(cacheKey.m_section, cacheKey.m_key))) {
      retVal = hr;
    }
  }

  return retVal;
}


//
// Check whether a certain setting changed
//
BOOL CLaunchPadSettings::IsDirty(LPCTSTR section, LPCTSTR key) const {
  SettingKey cacheKey(section, key);                  // the key used to access this setting in the settings cache
  SettingValue cacheValue(FALSE, FALSE);              // the cached setting value

  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    return FALSE;
  } else {
    return cacheValue.m_isChanged;
  }
}

//
// Check whether any setting changed
//
BOOL CLaunchPadSettings::IsDirty(void) const {
  SettingKey cacheKey;                                // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  POSITION pos = m_settingsCache.GetStartPosition();

  while (pos != NULL) {
    m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);

    if (cacheValue.m_isChanged) {
      return TRUE;
    }
  }

  return FALSE;
}

//
// Loads the data from a template file
//
HRESULT CLaunchPadSettings::LoadFromTemplate(
  LPCTSTR templateFile)                               // template settings (.ini) file
{
  return LoadFromTemplate(CLaunchPadSettings(templateFile));
}

HRESULT CLaunchPadSettings::LoadFromTemplate(
  const CLaunchPadSettings& templateSettings)         // template settings (.ini) file
{
  HRESULT hr = S_OK;

  CStringArray templateFiles;
  SettingKey   cacheKey;                              // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  // Clear the existing cache
  Reset();

  // Cache in all the template settings from disk
  templateSettings.GetFileNames(templateFiles);

  if (!GetAllStrings(templateFiles, m_settingsCache))
    hr = E_FAIL;  // something went wrong with at least one setting; continue with the process, though

  // Merge in any uncommited changes from templateSettings
  POSITION pos = templateSettings.m_settingsCache.GetStartPosition();

  while (pos != NULL) {
    templateSettings.m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);

    if (cacheValue.m_isChanged) {
      ASSERT(!cacheValue.m_isIndeterminate);
      m_settingsCache.SetAt(cacheKey, cacheValue);
    }
  }

  // Mark everything as dirty so that everything
  //  gets written at the next commit
  if (FAILED(MakeDirty()))
    hr = E_FAIL;

  return hr;
}

//
// Use this member function to copy the settings of one object to another
//
void CLaunchPadSettings::Copy(
  const CLaunchPadSettings& src)                      // source object
{
  // Make a copy of the list of file names the settings are stored in
  m_fileNames.Copy(src.m_fileNames);

  // Make a copy of the settings
  SettingKey cacheKey;                                // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  m_settingsCache.RemoveAll();                        // purge the old settings

  POSITION pos = src.m_settingsCache.GetStartPosition();

  while (pos != NULL) {
    src.m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);
    m_settingsCache.SetAt(cacheKey, cacheValue);
  }
}

//
// Use this member function to clear the settings
//
void CLaunchPadSettings::Reset(void) {
  // Clear the settings
  m_settingsCache.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
//
//
BOOL CLaunchPadSettings::GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, CString& result, LPCTSTR lpFileName) {
  CString tmpBuf, tmpDefault;

  TranslateTo(lpDefault, tmpDefault);                 // translate from plain text

  LPTSTR szTmpBuf = tmpBuf.GetBuffer(TMP_BUF_SIZE);   // obtain direct access to result's internal storage (reserve TMP_BUF_SIZE characters)

  ASSERT(szTmpBuf != NULL);

  ::GetPrivateProfileString(lpAppName, lpKeyName, tmpDefault, szTmpBuf, TMP_BUF_SIZE, lpFileName);

  tmpBuf.ReleaseBuffer();                             // give tmpBuf back control of its internal storage

  TranslateFrom(tmpBuf, result);                      // translate into plain text

  result.TrimLeft();                                  // trim spaces
  result.TrimRight();                                 // trim spaces

  return TRUE;
}

//
//
//
BOOL CLaunchPadSettings::WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName) {
  CString tmpBuf;

  TranslateTo(lpString, tmpBuf);                      // translate from plain text

  return ::WritePrivateProfileString(lpAppName, lpKeyName, tmpBuf, lpFileName);
}

//
//
//
BOOL CLaunchPadSettings::GetPrivateProfileSectionNames(CStringArray& result, LPCTSTR lpFileName) {
  CString tmpBuf;

  result.RemoveAll();

  LPTSTR szTmpBuf = tmpBuf.GetBuffer(TMP_BUF_SIZE);   // obtain direct access to result's internal storage (reserve TMP_BUF_SIZE characters)

  ASSERT(szTmpBuf != NULL);

  DWORD bufLen = ::GetPrivateProfileSectionNames(szTmpBuf, TMP_BUF_SIZE, lpFileName);

  if (bufLen < TMP_BUF_SIZE - 2) {
    LPCTSTR bufPtr;

    for (bufPtr = szTmpBuf; bufPtr[0] != _T('\0'); bufPtr += _tcslen(bufPtr) + 1) {
      ASSERT((UINT)(bufPtr - szTmpBuf) < bufLen);
      result.Add(bufPtr);
    }

    ASSERT((UINT)(bufPtr - szTmpBuf) == bufLen);

    tmpBuf.ReleaseBuffer();                           // give tmpBuf back control of its internal storage

    return TRUE;
  } else {
    tmpBuf.ReleaseBuffer();                           // give tmpBuf back control of its internal storage

    return FALSE;
  }
}

//
//
//
BOOL CLaunchPadSettings::GetPrivateProfileSection(LPCTSTR lpAppName, CStringArray& result, LPCTSTR lpFileName) {
  CString tmpBuf;

  result.RemoveAll();

  LPTSTR szTmpBuf = tmpBuf.GetBuffer(TMP_BUF_SIZE);   // obtain direct access to result's internal storage (reserve TMP_BUF_SIZE characters)

  ASSERT(szTmpBuf != NULL);

  DWORD bufLen = ::GetPrivateProfileSection(lpAppName, szTmpBuf, TMP_BUF_SIZE, lpFileName);

  if (bufLen < TMP_BUF_SIZE - 2) {
    LPCTSTR bufPtr;

    for (bufPtr = szTmpBuf; bufPtr[0] != _T('\0'); bufPtr += _tcslen(bufPtr) + 1) {
      ASSERT((UINT)(bufPtr - szTmpBuf) < bufLen);
      result.Add(bufPtr);
    }

    ASSERT((UINT)(bufPtr - szTmpBuf) == bufLen);

    tmpBuf.ReleaseBuffer();                           // give tmpBuf back control of its internal storage

    return TRUE;
  } else {
    tmpBuf.ReleaseBuffer();                           // give tmpBuf back control of its internal storage

    return FALSE;
  }
}

//
// Read in all the values from the .ini file and caches them
//
BOOL CLaunchPadSettings::GetAllStrings(const CStringArray& fileNames, CSettingsMap& settingsCache) {
  BOOL retVal = TRUE;
  CString tmpBuf;

  // Go through all configuration files
  for (int i = 0; i < fileNames.GetSize(); i++) {
    CString fileName = fileNames.GetAt(i);
    CStringArray sections;

    if (!GetPrivateProfileSectionNames(sections, fileName))
      retVal = FALSE;

    for (int j = 0; j < sections.GetSize(); j++) {
      CString section = sections.GetAt(j);
      CStringArray keyvalues;

      if (!GetPrivateProfileSection(section, keyvalues, fileName))
        retVal = FALSE;

      for (int k = 0; k < keyvalues.GetSize(); k++) {
        CString keyvalue = keyvalues.GetAt(k);
        int splitPos = keyvalue.Find(_T('='));

        if (splitPos <= 0) {
          retVal = FALSE;
          continue;
        }

        CString key, value;

        key = keyvalue.Left(splitPos);

        TranslateFrom(keyvalue.Mid(splitPos + 1), value); // translate into plain text
        value.TrimLeft();                                 // trim spaces
        value.TrimRight();                                // trim spaces

        SettingKey cacheKey(section, key);            // the key used to access this setting in the settings cache
        SettingValue cacheValue(FALSE, FALSE, value); // temporary storage for the setting

        // If another key-value pair was already found under the given section in another
        //  file then check if it is different
        if ((settingsCache.Lookup(cacheKey, cacheValue)) &&
            (value.CollateNoCase(cacheValue.m_value) != 0))
        {
          cacheValue.m_isIndeterminate = TRUE;
        }

        settingsCache.SetAt(cacheKey, cacheValue);
      }
    }
  }

  return TRUE;
}

//
//
//
VOID CLaunchPadSettings::TranslateTo(LPCTSTR plain, CString& translated) {
  CString tmpBuf;

  int plainLen = (int)_tcslen(plain);

  translated.Empty();

  for (int i = 0; i < plainLen; i++) {
    if (NeedsTranslation(plain[i])) {
      tmpBuf.Format(_T("%c%04x"), _T(META_CHAR), plain[i] & 0xffff);
      translated += tmpBuf;
    } else {
      translated += plain[i];
    }
  }
}

//
//
//
VOID CLaunchPadSettings::TranslateFrom(LPCTSTR translated, CString& plain) {
  int tmpStart, tmpStop;
  WCHAR tmpWChr;
# ifndef _UNICODE
  CHAR tmpMBChr[8];
# endif // _UNICODE

  TCHAR tmpBuf[META_LEN + 1];
  memset(tmpBuf, 0, sizeof(tmpBuf));

  int transLen = (int)_tcslen(translated);

  plain.Empty();

  for (int i = 0; i < transLen; i++) {
    switch (translated[i]) {
      case _T(META_CHAR):
        tmpStart = min(i + 1,        transLen - 1);
        tmpStop  = min(i + META_LEN, transLen - 1);

        _tcsncpy(tmpBuf, &(translated[tmpStart]), tmpStop - tmpStart + 1);

        tmpWChr  = (WCHAR)_tcstoul(tmpBuf, NULL, 16);
#       ifdef _UNICODE
        plain += tmpWChr;
#       else  // _UNICODE
        memset(tmpMBChr, 0, sizeof(tmpMBChr));
        WideCharToMultiByte(CP_ACP, 0, &tmpWChr, 1, tmpMBChr, sizeof(tmpMBChr) - 1, NULL, NULL);
        plain += tmpMBChr;
#       endif // _UNICODE

        i = tmpStop;
        break;

      case _T(COMMENT_CHAR):
        i = transLen - 1;
        break;

      default:
        plain += translated[i];
    }
  }
}

//
//
//
BOOL CLaunchPadSettings::NeedsTranslation(TCHAR tch) {
  if ((tch < ' ') || (tch > '~') || (tch == COMMENT_CHAR) || (tch == META_CHAR)) {
    return TRUE;
  } else {
    return FALSE;
  }
}
