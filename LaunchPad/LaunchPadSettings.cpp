#include "stdafx.h"
#include "LaunchPadSettings.h"

// MFC-specific stuff
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#define TMP_BUF_SIZE  16384
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

CLaunchPadSettings::CLaunchPadSettings(const CString& fileName) {
  // Keep a copy of the list of file names the settings are stored in
  m_fileNames.Add(fileName);
}

CLaunchPadSettings::CLaunchPadSettings(const CStringArray& fileNames) {
  // Keep a copy of the list of file names the settings are stored in
  m_fileNames.Copy(fileNames);
}

CLaunchPadSettings::CLaunchPadSettings(const CLaunchPadSettings& settings) {
  // Keep a copy of the list of file names the settings are stored in
  Copy(settings, FALSE);
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

//
// Obtain a setting value either from the list of configuration files
//  associated with the object at construction time.
//  The firts time a value is read it is cached, and the cached value
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
    bool isFirstValue = true;   // whether reading values from the firts file in the list or not

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

  return doWriteThrough ? CommitValue(section, key) : S_OK;
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
// Commits changes to a certain setting to disk
//
HRESULT CLaunchPadSettings::CommitValue(
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

  return hr;
}

//
// Commits all changes to disk
//
HRESULT CLaunchPadSettings::CommitAll(void) {
  HRESULT hr, retVal = S_OK;
  SettingKey cacheKey;                                // temporary storage for the setting key
  SettingValue cacheValue;                            // temporary storage for the setting value

  POSITION pos = m_settingsCache.GetStartPosition();
  
  while (pos != NULL) {
    m_settingsCache.GetNextAssoc(pos, cacheKey, cacheValue);

    if (FAILED(hr = CommitValue(cacheKey.m_section, cacheKey.m_key))) {
      retVal = hr;
    }
  }

  return retVal;
}


//
// Check whether a certain setting changed
//
BOOL CLaunchPadSettings::IsChanged(LPCTSTR section, LPCTSTR key) {
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
BOOL CLaunchPadSettings::IsChanged(void) {
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
// Read in all the values from the .ini file and caches them
//
void CLaunchPadSettings::GetAll(void) {
  // TODO: implement
}

//
// Use this member function to copy the settings of one object to another
//
void CLaunchPadSettings::Copy(
  const CLaunchPadSettings& src,                      // source object
  BOOL bValuesOnly)                                   // if TRUE only copies the key/value pairs; if FALSE, filename information is also replaced
{
  // Make a copy of the list of file names the settings are stored in
  if (!bValuesOnly)
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
void CLaunchPadSettings::Reset(
  BOOL bValuesOnly)                                   // if TRUE only clears the key/value pairs; if FALSE, filename information is also cleared
{
  // Clear the list of file names the settings are stored in
  if (!bValuesOnly)
    m_fileNames.RemoveAll();

  // Clear the settings
  m_settingsCache.RemoveAll();                        // purge the old settings
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

  if (szTmpBuf == NULL)
    return FALSE;

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
