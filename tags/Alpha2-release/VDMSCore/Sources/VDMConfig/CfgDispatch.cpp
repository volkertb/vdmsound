#include "StdAfx.h"
#include "resource.h"

#include "VDMConfig.h"

#include "INIConfig.h"
#include "CfgQuery.h"
#include "CfgDispatch.h"

#include "Messages.h"
#include "Help/Help.h"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <INIParser.h>
#pragma comment ( lib , "INIParser.lib" )

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb>

/////////////////////////////////////////////////////////////////////////////

void InstantiateModule(const std::string&, const CVDMConfig&, IUnknownPtr&);
void InitializeModule(const std::string&, const CVDMConfig&, const modulemap_t&, const IUnknownPtr&);
void DestroyModule(const IUnknownPtr&);
CString StringFromGUID(const GUID& guid);
CString FormatCOMError(const _com_error& ce);

class CInstantiationError : public CException {
  public:
    CInstantiationError(const CInstantiationError& src)
      : m_errText(src.m_errText), m_defaultText(src.m_defaultText)
    { }
    CInstantiationError(const CString& defaultText = _T(""))
      : CException(FALSE), m_defaultText(defaultText + "\n")
    { }
    void AddError(
        const CString& location,
        HRESULT errCode)
    {
      m_errText += FormatCOMError(_com_error(errCode)) + " (" + location + ")\n";
    }
    CString GetErrorMessage(void) const {
      return (m_errText.IsEmpty() ? m_defaultText : m_errText);
    }
    BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT /*pnHelpContext*/ = NULL) {
      _ASSERTE(lpszError != NULL);
      _ASSERTE(nMaxError > 0);
      _tcsncpy(lpszError, (LPCTSTR)GetErrorMessage(), nMaxError);
      lpszError[nMaxError - 1] = '\0';
      return TRUE;
    }
  protected:
    CString m_errText, m_defaultText;
};

/////////////////////////////////////////////////////////////////////////////

// The settings, as read from an INI file
CINIConfig config;

// Modules and their corresponding COM instances
modulemap_t modules;

// ordered sequence of module names; load & initialize modules in this order.
strvector_t orderedNames;


/////////////////////////////////////////////////////////////////////////////
//
// Exported functions (entrypoints)
//
/////////////////////////////////////////////////////////////////////////////

//
// Load and initialize all emulation modules
//
STDAPI CfgInitialize(void) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  HRESULT hr;

  // Initialize the COM framework
  if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
    MessageBox(FormatMessage(MSG_ERR_COMINIT, false, NULL, 0, false, hr, (LPCTSTR)FormatMessage(hr)),
               LoadString(IDS_MBT_COMINIT),
               MB_OK, MB_ICONERROR);
    return hr;
  }

  // Initialize and configure the emulation modules
  try {
    // Parse the configuration (INI) file
    try {
      config.load("vdms.ini");
    } catch (CINIParser::fopen_error& foe) {
      DWORD lastError = GetLastError();
      MessageBox(FormatMessage(MSG_ERR_IO_OPEN, false, NULL, 0, false, (LPCTSTR)CString(foe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)),
                 LoadString(IDS_MBT_IOERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIParser::io_error& ioe) {
      DWORD lastError = GetLastError();
      MessageBox(FormatMessage(MSG_ERR_IO, false, NULL, 0, false, (LPCTSTR)CString(ioe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)),
                 LoadString(IDS_MBT_IOERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIParser::syntax_error& se) {
      MessageBox(FormatMessage(MSG_ERR_PARSE_SYNTAX, false, NULL, 0, false, (LPCTSTR)CString(se.location.c_str()), (LPCTSTR)CString(se.offender.c_str()), (LPCTSTR)CString(se.expected.c_str())),
                 LoadString(IDS_MBT_PARSINGERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIParser::format_error& fe) {
      MessageBox(FormatMessage(MSG_ERR_PARSE_FORMAT, false, NULL, 0, false, (LPCTSTR)CString(fe.location.c_str())),
                 LoadString(IDS_MBT_PARSINGERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIParser::conflict_error& ce) {
      MessageBox(FormatMessage(MSG_ERR_PARSE_CONFLICT, false, NULL, 0, false, (LPCTSTR)CString(ce.location.c_str()), (LPCTSTR)CString(ce.defLocation.c_str()), (LPCTSTR)CString(ce.redefined.c_str())),
                 LoadString(IDS_MBT_PARSINGERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIParser::parse_error& pe) {
      MessageBox(FormatMessage(MSG_ERR_PARSE, false, NULL, 0, false, (LPCTSTR)CString(pe.location.c_str())),
                 LoadString(IDS_MBT_PARSINGERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    }

    // Perform a quick structural check (spots most spelling errors)
    try {
      config.validate();
    } catch (CINIConfig::modulename_error& mne) {
      MessageBox(FormatMessage(MSG_ERR_MODULENAME, false, NULL, 0, false, (LPCTSTR)CString(mne.sections.c_str())),
                 LoadString(IDS_MBT_VALIDATIONERROR),
                 MB_OK, MB_ICONERROR);
      return E_FAIL;
    } catch (CINIConfig::orphansection_error& ose) {
      if (MessageBox(FormatMessage(MSG_WRN_ORPHANS, false, NULL, 0, false, (LPCTSTR)CString(ose.sections.c_str())),
                     LoadString(IDS_MBT_VALIDATIONWARNING),
                     MB_YESNO|MB_DEFBUTTON1, MB_ICONWARNING) == IDNO)
      {
        return E_FAIL;
      }
    }

    // Get the order module should be initialized in
    try {
      config.getModuleOrder(orderedNames);
    } catch (CINIConfig::cyclic_error& ce) {
      if (MessageBox(FormatMessage(MSG_ERR_CYCLICDEP, false, NULL, 0, false, (LPCTSTR)CString(ce.cycle.c_str()), (LPCTSTR)CString(ce.modules.c_str())),
                     LoadString(IDS_MBT_DEPENDENCYERROR),
                     MB_YESNO|MB_DEFBUTTON2, MB_ICONERROR) == IDNO)
      {
        return E_FAIL;
      }
    }

    // Instantiate all COM modules
    for (strvector_t::const_iterator itName = orderedNames.begin(); itName != orderedNames.end(); itName++) {
      try {
        IUnknownPtr ptr;
        InstantiateModule(*itName, config, ptr);
        InitializeModule(*itName, config, modules, ptr);
        modules.insert(std::make_pair(*itName, ptr));
      } catch (CInstantiationError& ie) {
        if (MessageBox(FormatMessage(MSG_STR_MODULE_INST, false, NULL, 0, false, (LPCTSTR)CString(itName->c_str()), (LPCTSTR)ie.GetErrorMessage()),
                       LoadString(IDS_MBT_INST_COMERR),
                       MB_YESNO|MB_DEFBUTTON2, MB_ICONERROR) == IDNO)
        {
          return E_FAIL;
        }
      } catch (_com_error& ce) {
        if (MessageBox(FormatMessage(MSG_STR_MODULE_CONFIG, false, NULL, 0, false, (LPCTSTR)CString(itName->c_str()), (LPCTSTR)FormatCOMError(ce)),
                       LoadString(IDS_MBT_CONFIG_COMERR),
                       MB_YESNO|MB_DEFBUTTON2, MB_ICONERROR,
                       (TCHAR*)ce.HelpFile(), ce.HelpContext()) == IDNO)
        {
          return ce.Error();
        }
      }
    }
  } catch (std::exception& e) {
    MessageBox(FormatMessage(MSG_ERR_STL_EXCEPTION, false, NULL, 0, false, (LPCTSTR)CString(e.what())),
               LoadString(IDS_MBT_EXCEPTION),
               MB_OK, MB_ICONERROR);
    return E_FAIL;
  }

  return S_OK;
}

//
// Unload and destroy all emulation modules
//
STDAPI CfgDestroy(void) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // Destroy all COM modules
  for (strvector_t::reverse_iterator ritName = orderedNames.rbegin(); ritName != orderedNames.rend(); ritName++) {
    modulemap_t::iterator moduleIt = modules.find(*ritName);

    if (moduleIt == modules.end())
      continue;

    try {
      DestroyModule(moduleIt->second);
    } catch (_com_error& ce) {
      MessageBox(FormatMessage(MSG_STR_MODULE_DESTROY, false, NULL, 0, false, (LPCTSTR)CString(ritName->c_str()), (LPCTSTR)FormatCOMError(ce)),
                 LoadString(IDS_MBT_RELEASE_COMERR),
                 MB_OK, MB_ICONERROR,
                 (TCHAR*)ce.HelpFile(), ce.HelpContext());
    }

    modules.erase(moduleIt);
  }

  modules.clear();
  orderedNames.clear();
  config.clear();

  CoUninitialize();

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////////////////

//
// Attempts to instantiate a COM emulation module using one of several
//   methods
//
void InstantiateModule(
    const std::string& moduleName,
    const CVDMConfig& config,
    IUnknownPtr& ptr)
{
  HRESULT hr = E_FAIL;
  std::string loadMethodParam, location;

  config.getLocation(moduleName, CVDMConfig::SEC_LOADER, location);
  CInstantiationError errorMsg(FormatMessage(MSG_ERR_LOADMETHOD, false, NULL, 0, false, (LPCTSTR)CString(moduleName.c_str()), (LPCTSTR)CString(location.c_str())));

  // try to load by CLSID
  try {
    config.getValue(moduleName, CVDMConfig::SEC_LOADER, "CLSID", loadMethodParam);
    if (SUCCEEDED(hr = ptr.CreateInstance(loadMethodParam.c_str()))) {
      return;               // instantiation was successful
    } else {
      config.getLocation(moduleName, CVDMConfig::SEC_LOADER, "CLSID", location);
      errorMsg.AddError(CString(location.c_str()), hr);
    }
  } catch (CVDMConfig::nokey_error& /*nke*/) { }

  /* TODO: try other loading methods (e.g. explicitly call LoadLibrary,
     GetProcAddress(DllGetClassObject), DllGetClassObject */

  throw errorMsg;           // Failure
}

//
// Attempts to load the threshold for error-logging detail from the loader
//   section corresponding to the given module in the .INI file
//
int GetModuleLoggingDetail(
    const std::string& moduleName,
    const CVDMConfig& config)
{
  int threshold;
  std::string thresholdStr = "";

  try {
    config.getValue(moduleName, CVDMConfig::SEC_LOADER, "detail", thresholdStr);
  } catch (CVDMConfig::nokey_error& /*nke*/) { }

  if (sscanf(thresholdStr.c_str(), "%d", &threshold) != 1) {
    threshold = LOG_WARNING; // by default, don't log anything less severe than warnings
  }

  return threshold;
}

//
// Attempts to initialize a COM emulation module
//
void InitializeModule(
    const std::string& moduleName,
    const CVDMConfig& config,
    const modulemap_t& modules,
    const IUnknownPtr& ptr)
{
  IVDMMODULELib::IVDMBasicModulePtr pModule;
  pModule = ptr;

  // Check if IVDMBasicModule interface was supported
  if (pModule == NULL)
    return; // it's OK, but don't continue: this module can't accept a CfgQuery object

  // Create query object, and pass to module
  HRESULT hr;
  CComObject<CCfgQuery>* pCQ = NULL;
  IUnknown* pQuery = NULL;

  try {
    if (FAILED(hr = CComObject<CCfgQuery>::CreateInstance(&pCQ)))
      throw _com_error(hr);     // Failure

    _ASSERTE(pCQ != NULL);

    int loggingLevel = GetModuleLoggingDetail(moduleName, config);

    pCQ->Init(CfgEnvironment(moduleName, config, modules, loggingLevel));

    if (FAILED(hr = pCQ->QueryInterface(__uuidof(IUnknown), (void**)(&pQuery))))
      throw _com_error(hr);     // Failure

    _ASSERTE(pQuery != NULL);

    pModule->Init(pQuery);

    pQuery->Release();          // balance the AddRef in QueryInterface
  } catch (_com_error& /*ce*/) {
    if (pQuery != NULL) {
      // Release query object (avoid memory leaks)
      pQuery->Release();
      // Module failed Init, give it a chance to cleanup
      try { pModule->Destroy(); } catch (_com_error& /*ce*/) { }
    } else if (pCQ != NULL) {
      // Object was allocated but not AddRef'ed, so must delete manually
      delete pCQ;
    }

    throw;
  }
}

//
// Attempts to destroy a COM emulation module
//
void DestroyModule(
    const IUnknownPtr& ptr)
{
  IVDMMODULELib::IVDMBasicModulePtr pModule;
  pModule = ptr;

  // Check if IVDMBasicModule interface was supported
  if (pModule == NULL) {
    return;
  }

  // Destroy module (release resources, etc.)
  pModule->Destroy();
}

//
// Converts a GUID into a string
//
CString StringFromGUID(
    const GUID& guid)
{
  std::basic_string<wchar_t> buf;
  buf.reserve(39);

  while (StringFromGUID2(guid, buf.begin(), buf.capacity()) == 0) {
    buf.reserve(2 * buf.capacity());
  }

  return CString(buf.c_str());
}

//
// Pretty-prints the information contained in a _com_error
//   object
//
CString FormatCOMError(
    const _com_error& ce)
{
  CString errCode, errSrc, errDescr, retVal;

  const int wrap = 75;

  // Do the source
  errSrc = (TCHAR*)ce.Source();
  if (!errSrc.IsEmpty()) {
    FormatMessage(errSrc, LoadString(IDS_FCE_SOURCE), false, (LPCTSTR)errSrc, (LPCTSTR)StringFromGUID(ce.GUID()));
    WrapString(errSrc, wrap);
    retVal = retVal + errSrc + _T("\n\n");
  }

  // Do the error code
  FormatMessage(errCode, LoadString(IDS_FCE_HRESULT), false, ce.Error(), ce.ErrorMessage());
  WrapString(errCode, wrap);
  retVal = retVal + errCode;

  // Do the description
  errDescr = (TCHAR*)ce.Description();
  if (!errDescr.IsEmpty()) {
    WrapString(errDescr, wrap);
    retVal = retVal + _T("\n") + errDescr;
  }

  return retVal;
}
