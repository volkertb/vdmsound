// AdLibCtl.cpp : Implementation of CAdLibCtl
#include "stdafx.h"
#include "EmuAdLib.h"
#include "AdLibCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE     _T("The dependency module '%1' does not support the '%2' interface.%0")
#define MSG_ERR_NOINSTSLOTS   _T("Too many instances running (maximum %!d!1 allowed).%0")
#define MSG_ERR_OPLINITFAILED _T("Unable to initialize OPL software synthesizer.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_WAVEOUT       L"WaveOut"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_RATE          L"sampleRate"

/////////////////////////////////////////////////////////////////////////////

#define OPL_INTERNAL_FREQ     3600000   // The OPL operates at 3.6MHz

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

bool CAdLibCtl::isInstancesInitialized = false;
CAdLibCtl* CAdLibCtl::instances[MAX_INSTANCES];
CCriticalSection CAdLibCtl::OPLMutex;

/////////////////////////////////////////////////////////////////////////////
// CAdLibCtl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IIOHandler
  };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// IVDMBasicModule
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  HRESULT hr;

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends    = configuration; // Dependency query object
    Config     = configuration; // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain the AdLib settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x388, 16, false);
    m_sampleRate = CFG_Get(Config, INI_STR_RATE, 22050, 10, false);

    /** Get VDM services ***************************************************/

    // Obtain VDM Services instance
    IUnknownPtr VDMServices
               = Depends->Get(INI_STR_VDMSERVICES);
    m_BaseSrv  = VDMServices;   // Base services (registers, interrupts, etc)
    m_IOSrv    = VDMServices;   // I/O services (I/O port hooks)

    if (m_BaseSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMBaseServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);
    if (m_IOSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMIOServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

    /** Get modules ********************************************************/

    // Try to obtain an interface to a Wave-out module, use NULL if none available
    m_waveOut  = DEP_Get(Depends, INI_STR_WAVEOUT, NULL, false);
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  IVDMSERVICESLib::IIOHandler* pHandler = NULL; // IIOHandler interface to <this>

  // Register the I/O handlers
  try {
    // Obtain a COM IIOHandler interface on this C++ object
    if (FAILED(hr = QueryInterface(__uuidof(IVDMSERVICESLib::IIOHandler), (void**)(&pHandler))))
      throw _com_error(hr);     // Failure

    // Add this object as an I/O handler on the specified port range
    m_IOSrv->AddIOHook(m_basePort, 4, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

    pHandler->Release();        // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pHandler != NULL)
      pHandler->Release();      // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // Initialize the OPL software synthesizer
  if (FAILED(hr = OPLCreate(m_sampleRate)))
    return hr;

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("AdLibCtl initialized (base port = 0x%03x)"), m_basePort));

  return S_OK;
}

STDMETHODIMP CAdLibCtl::Destroy() {
  // release the OPL software synthesizer
  OPLDestroy();

  // Release the Wave-out module
  m_waveOut = NULL;

  // Release the VDM Services module
  m_IOSrv   = NULL;
  m_BaseSrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("AdLibCtl released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IIOHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::HandleINB(USHORT inPort, BYTE * data) {
  if (data == NULL)
    return E_POINTER;

  DWORD curTime = 0;

  switch (inPort - m_basePort) {
    case 0:   // the address/status port
      curTime = timeGetTime();

      if (m_targetTime[0].isActive && (m_targetTime[0].expiration <= curTime))
        MAME::OPLTimerOver(m_OPL, 0);   // The first timer expired

      if (m_targetTime[1].isActive && (m_targetTime[1].expiration <= curTime))
        MAME::OPLTimerOver(m_OPL, 1);   // The second timer expired

      *data = MAME::OPLRead(m_OPL, 0);
      return S_OK;

    case 1:   // the data port
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to read from write-only port (IN 0x%3x)"), inPort));
      *data = 0xff;
      return S_FALSE;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

STDMETHODIMP CAdLibCtl::HandleOUTB(USHORT outPort, BYTE data) {
  switch (outPort - m_basePort) {
    case 0:   // the address/status port
      // TODO: set register index
      MAME::OPLWrite(m_OPL, 0, data);
      return S_OK;

    case 1:   // the data port
      // TODO: set register data
      MAME::OPLWrite(m_OPL, 1, data);
      return S_OK;

    default:
      return E_FAIL;
  }
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::HandleINW(USHORT inPort, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleOUTW(USHORT outPort, USHORT data) {
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
//
//
HRESULT CAdLibCtl::OPLCreate(int sampleRate) {
  int i;

  // Lock static instances array and access to OPL software synthesizer
  CSingleLock lock(&OPLMutex, TRUE);

  // Initialize the static instances array (only once per DLL instance)
  if (!isInstancesInitialized) {
    isInstancesInitialized = true;

    for (i = 0; i < MAX_INSTANCES; i++) {
      instances[i] = NULL;
    }
  }

  // Look for an empty slot in the static instances array
  for (i = 0; i < MAX_INSTANCES; i++) {
    if (instances[i] == NULL) {
      m_instanceID = i;
      break;
    }
  }

  // If an empty slot was found take it, return an error otherwise
  if (m_instanceID < 0)
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_NOINSTSLOTS, /*false, NULL, 0, */false, (int)MAX_INSTANCES), __uuidof(IVDMBasicModule), E_OUTOFMEMORY);

  instances[m_instanceID] = this;

  // Initialize the OPL software synthesizer, return an error if failed
  if ((m_OPL = MAME::OPLCreate(OPL_TYPE_YM3812, OPL_INTERNAL_FREQ, sampleRate)) == NULL)
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_OPLINITFAILED, /*false, NULL, 0, */false), __uuidof(IVDMBasicModule), E_FAIL);

  // Install the callback handlers
  MAME::OPLSetTimerHandler(m_OPL, OPLTimerHandler, m_instanceID * 2);
  MAME::OPLSetUpdateHandler(m_OPL, OPLUpdateHandler, m_instanceID);

  m_targetTime[0].isActive = false;
  m_targetTime[1].isActive = false;

  return S_OK;
}

//
//
//
void CAdLibCtl::OPLDestroy(void) {
  // Lock static instances array and access to OPL software synthesizer
  CSingleLock lock(&OPLMutex, TRUE);

  // Release the OPL software synthesizer
  if (m_OPL != NULL)
    MAME::OPLDestroy(m_OPL);

  // Release the slot in the static instance array
  if (m_instanceID >= 0)
    instances[m_instanceID] = NULL;
}



/////////////////////////////////////////////////////////////////////////////
// OPL callback functions
/////////////////////////////////////////////////////////////////////////////

//
//
//
void CAdLibCtl::OPLTimerHandler(int channel, double interval_sec) {
  CAdLibCtl* pThis = instances[channel / 2];
  if (pThis == NULL) return;

  TimerInfo& timerInfo = pThis->m_targetTime[channel % 2];

  if (interval_sec == 0.0) {
    timerInfo.isActive = false;
  } else {
    timerInfo.expiration = timeGetTime() + (DWORD)(interval_sec * 1.0e3);
    timerInfo.isActive = true;
  }
}

//
//
//
void CAdLibCtl::OPLUpdateHandler(int param, int min_interval_usec) {
  static DWORD lastTime = 0;  /* NO STATIC in final version (because of multimple COM instances with same DLL !!! */
  static double scale = 1.0;

  /* TODO: try doing all this crap from a thread, asynchronously */

  CAdLibCtl* pThis = instances[param];
  if (pThis == NULL) return;

  if (pThis->m_waveOut == NULL)
    return; // why bother if no renderer is attached ?

  DWORD curTime = timeGetTime();

  if (lastTime == 0) {
    lastTime = curTime;

    // Set up the renderer (if any)
    try {
      pThis->m_waveOut->SetFormat(1, pThis->m_sampleRate, 16);
    } catch (_com_error& ce) {
      CString args = Format(_T("%d, %d, %d"), 1, pThis->m_sampleRate, 16);
      RTE_RecordLogEntry(pThis->m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
    }
  } else if (curTime > lastTime) {
    MAME::INT16 buf[65536];
    int numBytes = (int)(pThis->m_sampleRate * ((curTime - lastTime) / 1000.0));
    numBytes = min(65536, numBytes);

    lastTime = curTime;

    MAME::YM3812UpdateOne(instances[param]->m_OPL, buf, numBytes);

    // Play the data, and update the load factor
    try {
      pThis->m_waveOut->PlayData((BYTE*)buf, 2*numBytes);
    } catch (_com_error& ce) {
      CString args = Format(_T("%p, %d"), buf, 2*numBytes);
      RTE_RecordLogEntry(pThis->m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
    }
  }
}
