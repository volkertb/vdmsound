#include "stdafx.h"
#include "LaunchPadSettings.h"

// MFC-specific stuff
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
//
// CLaunchPadSettings
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLaunchPadSettings::CLaunchPadSettings(const CStringArray& fileNames) {
  // Keep a copy of the list of file names the settings are stored in
  m_fileNames.Copy(fileNames);
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
      DWORD lastError;
      LPTSTR szTmpBuf = tmpBuf.GetBuffer(1024);       // obtain direct access to tmpBuf's internal storage (reserve 1024 characters)

      GetPrivateProfileString(section, key, defValue, szTmpBuf, 1024, m_fileNames.GetAt(i));
      lastError = GetLastError();

      tmpBuf.ReleaseBuffer();                         // give tmpBuf back control of its internal storage

      if (lastError != ERROR_SUCCESS)                 // if file access failed then abort now
        return HRESULT_FROM_WIN32(lastError);

      tmpBuf.Replace(_T(';'), _T('\0'));              // remove ';'-prefixed comments
      tmpBuf.TrimLeft();                              // trim spaces
      tmpBuf.TrimRight();                             // trim spaces

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

  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    ASSERT(FALSE);
    return E_INVALIDARG;
  }

  if ((cacheValue.m_isIndeterminate) || (cacheValue.m_value.Collate(value) != 0)) {
    cacheValue.m_isChanged = TRUE;
    cacheValue.m_isIndeterminate = FALSE;
    cacheValue.m_value = value;

    m_settingsCache.SetAt(cacheKey, cacheValue);

    return doWriteThrough ? CommitValue(section, key) : S_OK;
  } else {
    return S_FALSE;
  }
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

  if (!m_settingsCache.Lookup(cacheKey, cacheValue)) {
    ASSERT(FALSE);
    return E_INVALIDARG;
  }

  if (!cacheValue.m_isIndeterminate) {
    cacheValue.m_isChanged = FALSE;
    cacheValue.m_isIndeterminate = TRUE;

    m_settingsCache.SetAt(cacheKey, cacheValue);

    return S_OK;
  } else {
    return S_FALSE;
  }
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
