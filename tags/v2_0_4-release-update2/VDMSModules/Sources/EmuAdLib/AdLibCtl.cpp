// AdLibCtl.cpp : Implementation of CAdLibCtl
#include "stdafx.h"
#include "EmuAdLib.h"
#include "AdLibCtl.h"

/////////////////////////////////////////////////////////////////////////////

#define USE_OPL2  1
#define USE_OPL3  0

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

#define OPL_AUDIOBUF_SIZE     65536
#define OPL_INTERNAL_FREQ     3600000   // The OPL operates at 3.6MHz
#define OPL_NUM_CHIPS         1         // Number of OPL chips
#define OPL_CHIP0             0

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
    &IID_IIOHandler,
    &IID_IAddressable
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

  // Put the OPL in a known state
  m_AdLibFSM.reset();

  // Initialize the OPL software synthesizer
  if (FAILED(hr = OPLCreate(m_sampleRate)))
    return hr;

  // Create the playback thread (converts FM information to PCM)
  m_playbackThread.Create(this, _T("AdLib Playback"), true);      /* TODO: check that creation was successful */
  m_playbackThread.SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
  m_playbackThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("AdLibCtl initialized (base port = 0x%03x)"), m_basePort));

  return S_OK;
}

STDMETHODIMP CAdLibCtl::Destroy() {
  // Signal the playback thread to quit
  if (m_playbackThread.GetThreadHandle() != NULL)
    m_playbackThread.Cancel();

  // Release the OPL software synthesizer
  OPLDestroy();

  // Put the OPL in a known state
  m_AdLibFSM.reset();

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
  return OPLRead(inPort - m_basePort, data);
}

STDMETHODIMP CAdLibCtl::HandleOUTB(USHORT outPort, BYTE data) {
  return OPLWrite(outPort - m_basePort, data);
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
// IAddressable
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::HandleByteRead(ULONG address, BYTE * data) {
  return OPLRead((BYTE)address, data);
}

STDMETHODIMP CAdLibCtl::HandleByteWrite(ULONG address, BYTE data) {
  return OPLWrite((BYTE)address, data);
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CAdLibCtl::HandleWordRead(ULONG address, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CAdLibCtl::HandleWordWrite(ULONG address, USHORT data) {
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CAdLibCtl::Run(CThread& thread) {
  MSG message;
  OPLMessage OPLMsg;

  int activity;
  bool hasStarted = false;

  _ASSERTE(thread.GetThreadID() == m_playbackThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Playback thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  m_renderLoad = 1.00;                              // we start off perfectly calibrated

  while (true) {
    if (thread.GetMessage(&message, false)) {       // non-blocking message-"peek"
      switch (message.message) {
        case WM_QUIT:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Playback thread cancelled")));
          return 0;

        default:
          break;
      }
    } else {
      // Process any pending OPL writes
      for (activity = 0; m_OPLMsgQueue.get(OPLMsg); activity++) {
        if (!hasStarted) {                          // is this the first OPL access ?
          m_lastTime = OPLMsg.timestamp;            // the timeline starts *now*
          m_curTime  = OPLMsg.timestamp;
          hasStarted = true;                        // we're in business

          // Set up the renderer (if any)
          if (m_waveOut != NULL) try {
#if USE_OPL2
            m_waveOut->SetFormat(1, m_sampleRate, OPL_SAMPLE_BITS);
#elif USE_OPL3
            m_waveOut->SetFormat(2, m_sampleRate, OPL3_SAMPLE_BITS);
#endif
          } catch (_com_error& ce) {
            CString args = Format(_T("%d, %d, %d"), 1, m_sampleRate, 16);
            RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
          }
        } else {
          m_lastTime = m_curTime;
          m_curTime  = OPLMsg.timestamp;
        }

        // Program the OPL (functions will return after the UpdateHandler()
        //  callback has been called)
#if USE_OPL2
        MAME::YM3812Write(OPL_CHIP0, 0, OPLMsg.address);
        MAME::YM3812Write(OPL_CHIP0, 1, OPLMsg.value);
#elif USE_OPL3
        MAME::YMF262Write(OPL_CHIP0, 0, OPLMsg.address);
        MAME::YMF262Write(OPL_CHIP0, 1, OPLMsg.value);
#endif
      }

      // Did we process any OPL writes (was the OPL kept active) ?
      if ((hasStarted) && (activity == 0)) {
        // The OPL was not written to lately, but we must periodically force
        //  it to output audio data
        m_lastTime = m_curTime;
        m_curTime  = timeGetTime();                 // we need the output data up to this point (i.e. "now")
        OPLPlay(m_curTime - m_lastTime);            // generate and output the data for the last time interval
      }

      Sleep(30);
    }
  }

  DWORD lastError = GetLastError();
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Abnormal condition encountered while waiting on message queue:\n0x%08x - %s"), lastError, (LPCTSTR)FormatMessage(lastError)));

  return -2;  // abnormal thread termination (error in message fetch)
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
// This function will create an OPL emulation core.  It will also take care
//  of associating this CAdLibCtl instance with a unique numeric ID that can
//  then be easily looked up inside the OPL core callback functions.
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
#if USE_OPL2
  if (MAME::YM3812Init(OPL_NUM_CHIPS, OPL_INTERNAL_FREQ, sampleRate))
#elif USE_OPL3
  if (MAME::YMF262Init(OPL_NUM_CHIPS, OPL_INTERNAL_FREQ, sampleRate))
#endif
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_OPLINITFAILED, /*false, NULL, 0, */false), __uuidof(IVDMBasicModule), E_FAIL);

  // Install the callback handler(s)
#if USE_OPL2
  MAME::YM3812SetUpdateHandler(OPL_CHIP0, OPLUpdateHandler, m_instanceID);
#elif USE_OPL3
  MAME::YMF262SetUpdateHandler(OPL_CHIP0, OPLUpdateHandler, m_instanceID);
#endif

  return S_OK;
}

//
// This function will release and clean up after the OPL emulation core
//
void CAdLibCtl::OPLDestroy(void) {
  // Lock static instances array and access to OPL software synthesizer
  CSingleLock lock(&OPLMutex, TRUE);

  // Release the OPL software synthesizer
#if USE_OPL2
  MAME::YM3812Shutdown();
#elif USE_OPL3
  MAME::YMF262Shutdown();
#endif

  // Release the slot in the static instance array
  if (m_instanceID >= 0)
    instances[m_instanceID] = NULL;
}

//
// This function will output a certain amount of synthesized OPL audio data
//  to the output wave device
//
void CAdLibCtl::OPLPlay(DWORD deltaTime) {
  if (m_waveOut == NULL)
    return; // why bother if no renderer is attached ?

  if (m_curTime > m_lastTime) {
    // Compute by how much this transfer should be boosted or diminished, based on
    //  playback performance (feedback indicating playback buffer overrun/underrun)
    double scalingFactor = min(2.0, max(0.0, 1 / m_renderLoad));

    // Compute how many samples we should transfer
    long toTransfer = min(OPL_AUDIOBUF_SIZE, (long)(scalingFactor * m_sampleRate * (deltaTime / 1000.0)));

#if USE_OPL2
    MAME::OPLSAMPLE buf[OPL_AUDIOBUF_SIZE];

    MAME::YM3812UpdateOne(OPL_CHIP0, buf, toTransfer);
#elif USE_OPL3
    MAME::OPL3SAMPLE  buf_tmp[4][OPL_AUDIOBUF_SIZE];
    MAME::OPL3SAMPLE* buf_tbl[] = { buf_tmp[0], buf_tmp[1], buf_tmp[2], buf_tmp[3] };
    MAME::OPL3SAMPLE* buf = buf_tmp[2];

    MAME::YMF262UpdateOne(OPL_CHIP0, buf_tbl, toTransfer);

    for (int i = 0; i < toTransfer; i++) {
      buf[2 * i + 0] = buf_tmp[0][i];
      buf[2 * i + 1] = buf_tmp[1][i];
    }
#endif

    // Play the data, and update the load factor
    try {
      m_renderLoad = m_waveOut->PlayData((BYTE*)buf, toTransfer * sizeof(buf[0]));
    } catch (_com_error& ce) {
      CString args = Format(_T("%p, %d"), buf, toTransfer * sizeof(buf[0]));
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
    }
  }
}

//
// This function will read a value from one of the OPL ports
//
HRESULT CAdLibCtl::OPLRead(BYTE address, BYTE * data) {
  if (data == NULL)
    return E_POINTER;

  switch (address) {
    case 0:   // the address/status port
      *data = m_AdLibFSM.getStatus();
      return S_OK;

    case 1:   // the data port
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to read from write-only port (IN 0x%3x)"), address + m_basePort));
      *data = 0xff;
      return S_FALSE;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

//
// This function will write a value to one of the OPL ports
//
HRESULT CAdLibCtl::OPLWrite(BYTE address, BYTE data) {
  switch (address) {
    case 0:   // the address/status port
      m_AdLibFSM.setAddress(data);
      return S_OK;

    case 1:   // the data port
      m_AdLibFSM.putData(data);
      return S_OK;

    default:
      return E_FAIL;
  }
}



/////////////////////////////////////////////////////////////////////////////
// IMPU401HWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

void CAdLibCtl::setOPLReg(int address, int value) {
  OPLMessage msg;

  msg.timestamp = timeGetTime();
  msg.address   = address;
  msg.value     = value;

  m_OPLMsgQueue.put(msg);
}

OPLTime_t CAdLibCtl::getTimeMicros(void) {
  static bool isInitialized = false;
  static LARGE_INTEGER perfCountFreq;

  if (!isInitialized) {
    if (QueryPerformanceFrequency(&perfCountFreq)) {
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Using performance counter (%0.0f ticks/s) for AdLib timing"), (float)perfCountFreq.QuadPart));
    } else {
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, _T("Could not query performance counter; using millisecond-resolution functions instead"));
    }

    isInitialized = true;
  }

  if (perfCountFreq.QuadPart < 1000) {
    return timeGetTime() * (OPLTime_t)1000;
  } else {
    LARGE_INTEGER currentCount;
    VERIFY(QueryPerformanceCounter(&currentCount));
    return (currentCount.QuadPart * (OPLTime_t)1000000) / perfCountFreq.QuadPart;
  }
}

void CAdLibCtl::logError(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, (LPCTSTR)CString(message));
}

void CAdLibCtl::logWarning(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, (LPCTSTR)CString(message));
}

void CAdLibCtl::logInformation(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, (LPCTSTR)CString(message));
}



/////////////////////////////////////////////////////////////////////////////
// OPL callback function(s)
/////////////////////////////////////////////////////////////////////////////

//
// This handler is called just before the OPL state changes, giving us the
//  chance to render the immediately preceding portion of the audio stream
//  just before the operators are reprogrammed.
//
void CAdLibCtl::OPLUpdateHandler(int param, int min_interval_usec) {
  CAdLibCtl* pThis = instances[param];
  if (pThis == NULL) return;

  // Generate and output the data for the last time interval
  pThis->OPLPlay(pThis->m_curTime - pThis->m_lastTime);
}
