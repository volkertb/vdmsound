// CfgQuery.cpp : Implementation of CCfgQuery
#include "StdAfx.h"
#include "VDMConfig.h"

#include "INIConfig.h"
#include "CfgQuery.h"
#include "CfgDispatch.h"

#include "Messages.h"
#include "Help/Help.h"

#include <mmsystem.h>
#pragma comment ( lib , "winmm.lib" )

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

const char SUFFIX_DEPENDS[]  = ".depends";
const char SUFFIX_CONFIG[]   = ".config";
const char PREFIX_DEFAULTS[] = "*";

/////////////////////////////////////////////////////////////////////////////
// CCfgQuery

CCfgQuery::CCfgQuery() : m_pEnv(NULL) {
}

CCfgQuery::~CCfgQuery() {
  if (m_pEnv != NULL)
    delete m_pEnv;
}

void CCfgQuery::Init(const CfgEnvironment& env) {
  if (m_pEnv != NULL)
    delete m_pEnv;
  m_pEnv = new CfgEnvironment(env);
}



/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CCfgQuery::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMQueryDependencies,
    &IID_IVDMQueryConfiguration,
    &IID_IVDMRTEnvironment
  };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IVDMQueryDependencies
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CCfgQuery::Get(BSTR name, IUnknown ** dependency) {
  if (dependency == NULL)
    return E_POINTER;

  *dependency = NULL;

  if (m_pEnv == NULL)
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_NOT_INIT, false, NULL, 0, false, m_pEnv), HLP_ERR_QUERY_NOT_INIT, ::GetHelpPath(), __uuidof(IVDMQueryDependencies), E_UNEXPECTED);

  std::string moduleName;         // name of the actual dependency module (to be determined)
  _bstr_t keyName(name, true);    // label under which the dependency-requirement is listed

  try {
    // try to find the name of the dependency module given the label
    try {
      m_pEnv->config.getValue(m_pEnv->name, CVDMConfig::SEC_DEPENDS, (const char*)keyName, moduleName);
    } catch (CVDMConfig::nosection_error& nse) {
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_DEP_INISECTION, false, NULL, 0, false, (LPCTSTR)CString(m_pEnv->name.c_str()), (LPCTSTR)CString(nse.section.c_str())), HLP_ERR_QUERY_DEP_INISECTION, ::GetHelpPath(), __uuidof(IVDMQueryDependencies), E_ABORT);
    } catch (CVDMConfig::nokey_error& nke) {
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_DEP_INIKEY, false, NULL, 0, false, (LPCTSTR)CString(m_pEnv->name.c_str()), (LPCTSTR)CString(nke.section.c_str()), (LPCTSTR)CString(nke.key.c_str())/*(TCHAR*)keyName*/, (LPCTSTR)CString(nke.location.c_str()), (LPCTSTR)CString(_T("*.depends"))), HLP_ERR_QUERY_DEP_INIKEY, ::GetHelpPath(), __uuidof(IVDMQueryDependencies), E_ABORT);
    }

    // given the module name, obtain the interface
    modulemap_t::const_iterator itModule;
    if ((itModule = m_pEnv->modules.find(moduleName)) == m_pEnv->modules.end()) {
      std::string referenceLocation;
      m_pEnv->config.getLocation(m_pEnv->name, CVDMConfig::SEC_DEPENDS, (const char*)keyName, referenceLocation);
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_DEP_INST, false, NULL, 0, false, (LPCTSTR)CString(m_pEnv->name.c_str()), (LPCTSTR)CString(moduleName.c_str()), (TCHAR*)keyName, (LPCTSTR)CString(referenceLocation.c_str())), HLP_ERR_QUERY_DEP_INST, ::GetHelpPath(), __uuidof(IVDMQueryDependencies), E_ABORT);
    }

    // copy the interface pointer (if non-null) & take care of AddRef()
    *dependency = itModule->second.GetInterfacePtr();

    if ((*dependency) != NULL) {
      (*dependency)->AddRef();
    }
  } catch (std::exception& e) {
    return AtlReportError(GetObjectCLSID(), ::FormatMessage(MSG_ERR_STL_EXCEPTION, false, NULL, 0, false, (LPCTSTR)CString(e.what())), /*HLP_ERR_QUERY_CFG_INIKEY, ::GetHelpPath(), */__uuidof(IVDMQueryDependencies), E_UNEXPECTED);
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMQueryConfiguration
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CCfgQuery::Get(BSTR name, BSTR * value) {
  if (value == NULL)
    return E_POINTER;

  *value = NULL;

  if (m_pEnv == NULL)
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_NOT_INIT, false, NULL, 0, false, m_pEnv), HLP_ERR_QUERY_NOT_INIT, ::GetHelpPath(), __uuidof(IVDMQueryConfiguration), E_UNEXPECTED);

  std::string setting;            // value of the setting string to be retrieved
  _bstr_t keyName(name, true);    // label under which the setting can be found

  try {
    // try to find the setting string given the label
    try {
      m_pEnv->config.getValue(m_pEnv->name, CVDMConfig::SEC_CONFIG, (const char*)keyName, setting);
    } catch (CVDMConfig::nosection_error& nse) {
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_CFG_INISECTION, false, NULL, 0, false, (LPCTSTR)CString(m_pEnv->name.c_str()), (LPCTSTR)CString(nse.section.c_str())), HLP_ERR_QUERY_CFG_INISECTION, ::GetHelpPath(), __uuidof(IVDMQueryConfiguration), E_ABORT);
    } catch (CVDMConfig::nokey_error& nke) {
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_CFG_INIKEY, false, NULL, 0, false, (LPCTSTR)CString(m_pEnv->name.c_str()), (LPCTSTR)CString(nke.section.c_str()), (LPCTSTR)CString(nke.key.c_str())/*(TCHAR*)keyName*/, (LPCTSTR)CString(nke.location.c_str()), (LPCTSTR)CString(_T("*.config"))), HLP_ERR_QUERY_CFG_INIKEY, ::GetHelpPath(), __uuidof(IVDMQueryConfiguration), E_ABORT);
    }

    // ANSI-2-UNICODE, then copy the setting string
    _bstr_t retVal(setting.c_str());
    *value = retVal.copy();
  } catch (std::exception& e) {
    return AtlReportError(GetObjectCLSID(), ::FormatMessage(MSG_ERR_STL_EXCEPTION, false, NULL, 0, false, (LPCTSTR)CString(e.what())), /*HLP_ERR_QUERY_CFG_INIKEY, ::GetHelpPath(), */__uuidof(IVDMQueryConfiguration), E_UNEXPECTED);
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMRTEnvironment
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CCfgQuery::RecordLogEntry(LOGENTRY_T type, BSTR message) {
  if (m_pEnv == NULL)
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_QUERY_NOT_INIT, false, NULL, 0, false, m_pEnv), HLP_ERR_QUERY_NOT_INIT, ::GetHelpPath(), __uuidof(IVDMRTEnvironment), E_UNEXPECTED);

  if (type < m_pEnv->loggingLevel)
    return S_FALSE; // do not need to log anything (filtered out)

  CString logEntry;
  CString entryType;
  CString logMsg = message;
  CString moduleName = (m_pEnv != NULL) ? m_pEnv->name.c_str() : "<error>";

  if (type >= LOG_ERROR) {
    entryType = _T("@E");
  } else if (type >= LOG_WARNING) {
    entryType = _T("@W");
  } else if (type >= LOG_INFORMATION) {
    entryType = _T("@I");
  } else {
    entryType = _T("@?");
  }

  WrapString(logMsg, 70, _T("\t"), _T("\t"));
  logEntry.Format(_T("%s - %s - %s\n%s\n"), (LPCTSTR)entryType, (LPCTSTR)FormatTime(timeGetTime()), (LPCTSTR)moduleName, (LPCTSTR)logMsg);

  FILE* fLog = fopen("vdms.log", "at");

  if (fLog != NULL) {
    _fputts((LPCTSTR)logEntry, fLog);
    fclose(fLog);
  }

  return S_OK;
}

CString CCfgQuery::FormatTime(DWORD millis) {
  CString Buffer;

  int mil = (millis % 1000);
  int sec = (millis / 1000) % 60;
  int min = (millis / (60 * 1000)) % 60;
  int hr  = (millis / (60 * 60 * 1000)) % 24;

  Buffer.Format(_T("%02d:%02d:%02d.%03d"), hr, min, sec, mil);
  return Buffer;
}

/* TODO: add true enumerator (IEnumXXX) functionality (Reset, Next) to the
   Dependency and Configuration query objects. */

/* TODO: add filtering and per-module configuration to the R.T. Environment's
   logging functions; the logging configuration (in what file, how detailed,
   etc.) should be taken from the [ModuleName] section, i.e. the same section
   used by the loader to get the module's CLSID and so on. */
