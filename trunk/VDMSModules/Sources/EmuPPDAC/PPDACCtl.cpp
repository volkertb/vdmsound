// PPDACCtl.cpp : Implementation of CPPDACCtl
#include "stdafx.h"
#include "EmuPPDAC.h"
#include "PPDACCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE     _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_WAVEOUT       L"WaveOut"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_RATE          L"sampleRate"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CPPDACCtl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CPPDACCtl::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CPPDACCtl::Init(IUnknown * configuration) {
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

    // Try to obtain the PPDAC settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x378, 16, false);
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
    m_IOSrv->AddIOHook(m_basePort, 3, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

    pHandler->Release();        // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pHandler != NULL)
      pHandler->Release();      // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // Create the playback thread (resamples PCM information)
  m_playbackThread.Create(this, _T("PPDAC Playback"), true);      /* TODO: check that creation was successful */
  m_playbackThread.SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
  m_playbackThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("PPDACCtl initialized (base port = 0x%03x)"), m_basePort));

  return S_OK;
}

STDMETHODIMP CPPDACCtl::Destroy() {
  // Signal the playback thread to quit
  if (m_playbackThread.GetThreadHandle() != NULL)
    m_playbackThread.Cancel();

  // Release the Wave-out module
  m_waveOut = NULL;

  // Release the VDM Services module
  m_IOSrv   = NULL;
  m_BaseSrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("PPDACCtl released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IIOHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CPPDACCtl::HandleINB(USHORT inPort, BYTE * data) {
  // TODO; actually put some fake state machine behind the parallel-port controller
  static BYTE inVals[] = { 0xaa, 0xdf, 0xe7 };

  BYTE buf[3];

  ULONG CS, EIP;

  switch (inPort - m_basePort) {
    case 0:
    case 1:
    case 2:
      *data = inVals[inPort - m_basePort];

      CS  = m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_CS);
      EIP = m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_EIP);

      m_BaseSrv->GetMemory((WORD)CS, EIP - 2, IVDMSERVICESLib::ADDR_V86, buf, 3);

      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Performance-degrading read from port 0x%03x at %04x:%08x (... %02x %02x > %02x ...), patching"), inPort & 0xff, CS & 0xffff, EIP & 0xffffffffl, buf[0] & 0xff, buf[1] & 0xff, buf[2] & 0xff));

      if ((inPort < 0x100) && ((buf[1] & 0xff) == (inPort & 0xff))) {
        if ((buf[0] == 0xe4) || (buf[0] == 0xe5)) {
          buf[0] = 0x90; // NOP
          buf[1] = 0x90; // NOP
          m_BaseSrv->SetMemory((WORD)CS, EIP - 2, IVDMSERVICESLib::ADDR_V86, &(buf[0]), 2);
          return S_OK;
        } else {
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid 'IN immed8' opcode (0x%02x) when reading from 0x%03x, cannot patch"), buf[0] & 0xff, inPort & 0xff));
          return S_FALSE;
        }
      } else {
        if ((buf[1] == 0xec) || (buf[1] == 0xed) || (buf[1] == 0x6c) || (buf[1] == 0x6d)) {
          buf[1] = 0x90; // NOP
          m_BaseSrv->SetMemory((WORD)CS, EIP - 1, IVDMSERVICESLib::ADDR_V86, &(buf[1]), 1);
          return S_OK;
        } else {
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid 'IN'/'INS' opcode (0x%02x) when reading from 0x%03x, cannot patch"), buf[1] & 0xff, inPort & 0xff));
          return S_FALSE;
        }
      }

    default:
      *data = 0xff;
      return S_FALSE;
  }
}

STDMETHODIMP CPPDACCtl::HandleOUTB(USHORT outPort, BYTE data) {
  BYTE buf[3];

  ULONG CS, EIP;

  int numSamples;

  switch (outPort - m_basePort) {
    case 0:
      m_lastTime = m_curTime;
      m_curTime  = getTimeMicros();

      if (m_lastTime == 0)
        m_lastTime = m_curTime;

      numSamples = max(0, min((int)sizeof(m_buffer) - m_bufPtr - 1, (int)(min(2.0, max(0.0, 1 / m_renderLoad)) * m_sampleRate * 1e-6 * (m_curTime - m_lastTime))));

      m_lock.Lock(100);
      memset((void*)(&(m_buffer[m_bufPtr])), data & 0xff, numSamples);
      m_bufPtr += numSamples;
      m_lock.Unlock();

      return S_OK;

    case 1:
    case 2:
      CS  = m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_CS);
      EIP = m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_EIP);

      m_BaseSrv->GetMemory((WORD)CS, EIP - 2, IVDMSERVICESLib::ADDR_V86, buf, 3);

      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Performance-degrading write to port 0x%03x at %04x:%08x (... %02x %02x > %02x ...), patching"), outPort & 0xff, CS & 0xffff, EIP & 0xffffffffl, buf[0] & 0xff, buf[1] & 0xff, buf[2] & 0xff));

      if ((outPort < 0x100) && ((buf[1] & 0xff) == (outPort & 0xff))) {
        if ((buf[0] == 0xe6) || (buf[0] == 0xe7)) {
          buf[0] = 0x90; // NOP
          buf[1] = 0x90; // NOP
          m_BaseSrv->SetMemory((WORD)CS, EIP - 2, IVDMSERVICESLib::ADDR_V86, &(buf[0]), 2);
          return S_OK;
        } else {
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid 'OUT immed8' opcode (0x%02x) when writing to 0x%03x, cannot patch"), buf[0] & 0xff, outPort & 0xff));
          return S_FALSE;
        }
      } else {
        if ((buf[1] == 0xee) || (buf[1] == 0xef) || (buf[1] == 0x6e) || (buf[1] == 0x6f)) {
          buf[1] = 0x90; // NOP
          m_BaseSrv->SetMemory((WORD)CS, EIP - 1, IVDMSERVICESLib::ADDR_V86, &(buf[1]), 1);
          return S_OK;
        } else {
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid 'OUT'/'OUTS' opcode (0x%02x) when writing to 0x%03x, cannot patch"), buf[1] & 0xff, outPort & 0xff));
          return S_FALSE;
        }
      }

    default:
      return S_FALSE;
  }
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CPPDACCtl::HandleINW(USHORT inPort, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CPPDACCtl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CPPDACCtl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CPPDACCtl::HandleOUTW(USHORT outPort, USHORT data) {
  return E_NOTIMPL;
}
STDMETHODIMP CPPDACCtl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}
STDMETHODIMP CPPDACCtl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CPPDACCtl::Run(CThread& thread) {
  MSG message;

  _ASSERTE(thread.GetThreadID() == m_playbackThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Playback thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  m_renderLoad = 1.00;                              // we start off perfectly calibrated

  // Set up the renderer (if any)
  if (m_waveOut != NULL) try {
    m_waveOut->SetFormat(1, m_sampleRate, 8);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d, %d, %d"), 1, m_sampleRate, 16);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }

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
      m_lock.Lock(100);

      // Play the data, and update the load factor
      if (m_bufPtr > 0) try {
        m_renderLoad = m_waveOut->PlayData((BYTE*)(m_buffer), m_bufPtr);
      } catch (_com_error& ce) {
        CString args = Format(_T("%p, %d"), m_buffer, m_bufPtr);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
      }

      m_bufPtr = 0;
      m_lock.Unlock();

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
// 
//
__int64 CPPDACCtl::getTimeMicros(void) {
  static bool isInitialized = false;
  static LARGE_INTEGER perfCountFreq;

  if (!isInitialized) {
    if (QueryPerformanceFrequency(&perfCountFreq)) {
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Using performance counter (%0.0f ticks/s) for PPDAC timing"), (float)perfCountFreq.QuadPart));
    } else {
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, _T("Could not query performance counter; using millisecond-resolution functions instead"));
    }

    isInitialized = true;
  }

  if (perfCountFreq.QuadPart < 1000) {
    return timeGetTime() * (__int64)1000;
  } else {
    LARGE_INTEGER currentCount;
    VERIFY(QueryPerformanceCounter(&currentCount));
    return (currentCount.QuadPart * (__int64)1000000) / perfCountFreq.QuadPart;
  }
}
