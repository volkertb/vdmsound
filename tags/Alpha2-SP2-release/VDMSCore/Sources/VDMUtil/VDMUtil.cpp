#include "stdafx.h"
#include "VDMUtil.h"

/////////////////////////////////////////////////////////////////////////////
//
// Logging functions
//
/////////////////////////////////////////////////////////////////////////////

//
// Sets the runtime environment COM object
// Used for convenience only (no exceptions thrown).
//
void AFXAPI RTE_Set(
    IVDMQUERYLib::IVDMRTEnvironmentPtr& environment,
    IUnknown * configuration)
{
  try {
    environment = configuration;
  } catch (_com_error& /*ce*/) { }
}

//
// Records an entry in the log associated with the runtime environment.
// Used for convenience only (no exceptions thrown).
//
void AFXAPI RTE_RecordLogEntry(
    IVDMQUERYLib::IVDMRTEnvironmentPtr& environment,
    IVDMQUERYLib::LOGENTRY_T type,
    LPCTSTR message)
{
  try {
    environment->RecordLogEntry(type, message);
  } catch (_com_error& /*ce*/) { }
}



/////////////////////////////////////////////////////////////////////////////
//
// Dependency retrieval functions
//
/////////////////////////////////////////////////////////////////////////////

//
// Tries to get a dependency; returns a default value (and, optionally, logs
//  an error message) if  an error occurs.
// Used for convenience only (no exceptions thrown).
//
IUnknown* AFXAPI DEP_Get(
    IVDMQUERYLib::IVDMQueryDependenciesPtr& dependencies,
    _bstr_t name,
    IUnknown* defaultValue,
    bool isSilent)
{
  try {
    return dependencies->Get(name);
  } catch (_com_error& ce) {
    if (!isSilent) {
      CString errMsg;
      errMsg.Format(_T("Could not obtain the dependency '%s'; using a default value instead.  Description follows:\n0x%08x - %s\n%s"), (LPCTSTR)name, ce.Error(), ce.ErrorMessage(), (LPCTSTR)ce.Description());
      IVDMQUERYLib::IVDMRTEnvironmentPtr environment;
      RTE_Set(environment, dependencies);
      RTE_RecordLogEntry(environment, IVDMQUERYLib::LOG_WARNING, errMsg);
    } return defaultValue;
  }
}



/////////////////////////////////////////////////////////////////////////////
//
// Configuration retrieval functions
//
/////////////////////////////////////////////////////////////////////////////

//
// Tries to get a numerical configuration value; returns a default value
//  (and, optionally, logs an error message) if  an error occurs.
// Used for convenience only (no exceptions thrown).
//
int AFXAPI CFG_Get(
    IVDMQUERYLib::IVDMQueryConfigurationPtr& configuration,
    _bstr_t name,
    int defaultValue,
    int base,
    bool isSilent)
{
  LPCSTR formatStr;
  CString defaultValueStr;

  switch (base) {
    case 8:
      formatStr = "%o";
      defaultValueStr.Format(_T("0%o"), defaultValue);
      break;
    case 10:
      formatStr = "%d";
      defaultValueStr.Format(_T("%d"), defaultValue);
      break;
    case 16:
      formatStr = "%x";
      defaultValueStr.Format(_T("0x%x"), defaultValue);
      break;
    default:
      formatStr = "";
      defaultValueStr = _T("<invalid base>");
  }

  try {
    int intValue;
    _bstr_t strValue = configuration->Get(name);

    if (sscanf((LPCSTR)strValue, formatStr, &intValue) == 1) {
      return intValue;
    } else {
      if (!isSilent) {
        CString errMsg;
        errMsg.Format(_T("The setting '%s' has an invalid value ('%s'); it must be a base-%d numeral.  Using a default value of %s instead."), (LPCTSTR)name, (LPCTSTR)strValue, base, (LPCTSTR)defaultValueStr);
        IVDMQUERYLib::IVDMRTEnvironmentPtr environment;
        RTE_Set(environment, configuration);
        RTE_RecordLogEntry(environment, IVDMQUERYLib::LOG_WARNING, errMsg);
      } return defaultValue;
    }
  } catch (_com_error& ce) {
    if (!isSilent) {
      CString errMsg;
      errMsg.Format(_T("Could not obtain the setting '%s'; using a default value of %s instead.  Description follows:\n0x%08x - %s\n%s"), (LPCTSTR)name, (LPCTSTR)defaultValueStr, ce.Error(), ce.ErrorMessage(), (LPCTSTR)ce.Description());
      IVDMQUERYLib::IVDMRTEnvironmentPtr environment;
      RTE_Set(environment, configuration);
      RTE_RecordLogEntry(environment, IVDMQUERYLib::LOG_WARNING, errMsg);
    } return defaultValue;
  }
}

//
// Tries to get a literal configuration value; returns a default value
//  (and, optionally, logs an error message) if  an error occurs.
// Used for convenience only (no exceptions thrown).
//
_bstr_t AFXAPI CFG_Get(
    IVDMQUERYLib::IVDMQueryConfigurationPtr& configuration,
    _bstr_t name,
    _bstr_t defaultValue,
    bool isSilent)
{
  try {
    return configuration->Get(name);
  } catch (_com_error& ce) {
    if (!isSilent) {
      CString errMsg;
      errMsg.Format(_T("Could not obtain the setting '%s'; using a default value of %s instead.  Description follows:\n0x%08x - %s\n%s"), (LPCTSTR)name, (LPCTSTR)defaultValue, ce.Error(), ce.ErrorMessage(), (LPCTSTR)ce.Description());
      IVDMQUERYLib::IVDMRTEnvironmentPtr environment;
      RTE_Set(environment, configuration);
      RTE_RecordLogEntry(environment, IVDMQUERYLib::LOG_WARNING, errMsg);
    } return defaultValue;
  }
}
