// MPU401Ctl.cpp : Implementation of CMPU401Ctl
#include "stdafx.h"
#include "EmuMPU401.h"
#include "MPU401Ctl.h"

#include "MIDIConst.h"

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_MIDIOUT       L"MidiOut"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_IRQLINE       L"IRQ"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CMPU401Ctl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMPU401Ctl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IIOHandler,
    &IID_IMIDIEventHandler
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

STDMETHODIMP CMPU401Ctl::Init(IUnknown * configuration) {
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

    // Obtain VDM Services instance (if available)
    IUnknownPtr VDMServices
               = Depends->Get(INI_STR_VDMSERVICES);
    m_BaseSrv  = VDMServices;   // Base services (registers, interrupts, etc)
    m_IOSrv    = VDMServices;   // I/O services (I/O port hooks)

    // Try to obtain the MPU-401 settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x330, 16, false);
    m_IRQLine  = CFG_Get(Config, INI_STR_IRQLINE, 2, 10, false);

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut  = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, false);
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
    m_IOSrv->AddIOHook(m_basePort, 2, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

    pHandler->Release();        // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pHandler != NULL)
      pHandler->Release();      // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MPU401Ctl initialized (base port = 0x%03x, IRQ = %d)"), m_basePort, m_IRQLine));

  return S_OK;
}

STDMETHODIMP CMPU401Ctl::Destroy() {
  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the VDM Services module
  m_IOSrv   = NULL;
  m_BaseSrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MPU401Ctl released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IIOHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMPU401Ctl::HandleINB(USHORT inPort, BYTE * data) {
  if (data == NULL)
    return E_POINTER;

  switch (inPort - m_basePort) {
    case 0:   // the data port
      *data = m_MPUFSM.getData();
      return S_OK;

    case 1:   // the command/status port
      *data = m_MPUFSM.getStatus();
      return S_OK;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

STDMETHODIMP CMPU401Ctl::HandleOUTB(USHORT outPort, BYTE data) {
  switch (outPort - m_basePort) {
    case 0:   // the data port
      m_MPUFSM.putData(data);
      return S_OK;

    case 1:   // the command/status port
      m_MPUFSM.putCommand(data);
      return S_OK;

    default:
      return E_FAIL;
  }
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CMPU401Ctl::HandleINW(USHORT inPort, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CMPU401Ctl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CMPU401Ctl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CMPU401Ctl::HandleOUTW(USHORT outPort, USHORT data) {
  return E_NOTIMPL;
}
STDMETHODIMP CMPU401Ctl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}
STDMETHODIMP CMPU401Ctl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMPU401Ctl::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  return S_OK;
}

STDMETHODIMP CMPU401Ctl::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
  if (data == NULL)
    return E_POINTER;

  return S_OK;
}

STDMETHODIMP CMPU401Ctl::HandleRealTime(LONGLONG usDelta, BYTE data) {
  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMPU401HWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

void CMPU401Ctl::generateInterrupt(void) {
  try {
    m_BaseSrv->SimulateInterrupt(IVDMSERVICESLib::INT_MASTER, m_IRQLine, 1);
  } catch (_com_error& ce) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SimulateInterrupt: 0x%08x - %s"), ce.Error(), ce.ErrorMessage()));
  }
}

void CMPU401Ctl::logMessage(msgType type, const char* message) {
  IVDMQUERYLib::LOGENTRY_T logType;

  switch (type) {
    case IMPU401HWEmulationLayer::MSG_INFO:
      logType = IVDMQUERYLib::LOG_INFORMATION; break;
    case IMPU401HWEmulationLayer::MSG_WARNING:
      logType = IVDMQUERYLib::LOG_WARNING; break;
    case IMPU401HWEmulationLayer::MSG_ERROR:
      logType = IVDMQUERYLib::LOG_ERROR; break;
    default:
      logType = IVDMQUERYLib::LOG_ERROR;
  }

  RTE_RecordLogEntry(m_env, logType, (LPCTSTR)CString(message));
}

void CMPU401Ctl::putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length) {
  if (m_midiOut == NULL)
    return;

  _ASSERTE(IS_MIDI_EVENT(status));
  _ASSERTE(!IS_REALTIME_EVENT(status));
  _ASSERTE(status != MIDI_EVENT_SYSTEM_SYSEX);
  _ASSERTE(status != MIDI_EVENT_SYSTEM_EOX);

  HRESULT hr;

  if (FAILED(hr = m_midiOut->HandleEvent(-1, status, data1, data2, length))) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("HandleEvent: 0x%08x - %s"), hr, (LPCTSTR)FormatMessage(hr)));
  }
}

void CMPU401Ctl::putSysEx(const unsigned char * data, long length) {
  if (m_midiOut == NULL)
    return;

  _ASSERTE(length >= 4);

  HRESULT hr;

  if (FAILED(hr = m_midiOut->HandleSysEx(-1, (BYTE*)(data), length))) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("HandleSysEx: 0x%08x - %s"), hr, (LPCTSTR)FormatMessage(hr)));
  }
}

void CMPU401Ctl::putRealTime(unsigned char data) {
  if (m_midiOut == NULL)
    return;

  _ASSERTE(IS_REALTIME_EVENT(data));

  HRESULT hr;

  if (FAILED(hr = m_midiOut->HandleRealTime(-1, data))) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("HandleRealTime: 0x%08x - %s"), hr, (LPCTSTR)FormatMessage(hr)));
  }
}
