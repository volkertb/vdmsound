// SBCompatCtl.cpp : Implementation of CSBCompatCtl
#include "stdafx.h"
#include "EmuSBCompat.h"
#include "SBCompatCtl.h"

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_IRQLINE       L"IRQ"
#define INI_STR_DMA8CHANNEL   L"DMA8"
#define INI_STR_DMA16CHANNEL  L"DMA16"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CSBCompatCtl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CSBCompatCtl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IIOHandler,
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

STDMETHODIMP CSBCompatCtl::Init(IUnknown * configuration) {
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

    // Try to obtain the SB settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x220, 16, false);
    m_IRQLine  = CFG_Get(Config, INI_STR_IRQLINE, 7, 10, false);
    m_DMA8Channel  = CFG_Get(Config, INI_STR_DMA8CHANNEL, 1, 10, false);
    m_DMA16Channel = CFG_Get(Config, INI_STR_DMA16CHANNEL, 5, 10, false);
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
    m_IOSrv->AddIOHook(m_basePort, 16, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

    pHandler->Release();        // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pHandler != NULL)
      pHandler->Release();      // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // put the DSP in a known state
  m_SBDSP.reset();

  // put the mixer in a known state
  m_SBMixer.reset();
  m_SBMixer.setIRQSelect(m_IRQLine);
  m_SBMixer.setDMASelect(m_DMA8Channel, m_DMA16Channel);

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("SBCompatCtl initialized (base port = 0x%03x, IRQ = %d, 8-bit DMA = %d, 16-bit DMA = %d)"), m_basePort, m_IRQLine, m_DMA8Channel, m_DMA16Channel));

  return S_OK;
}

STDMETHODIMP CSBCompatCtl::Destroy() {
  // put the mixer in a known state
  m_SBMixer.reset();

  // put the DSP in a known state
  m_SBDSP.reset();

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

STDMETHODIMP CSBCompatCtl::HandleINB(USHORT inPort, BYTE * data) {
  if (data == NULL)
    return E_POINTER;

  switch (inPort - m_basePort) {
    case 0x05:  /* MIXER data register */
      *data = m_SBMixer.getValue();
      return S_OK;

    case 0x0a:  /* DSP read data */
      *data = m_SBDSP.getData();
      return S_OK;

    case 0x0c:  /* DSP write buffer status */
      *data = m_SBDSP.getWrStatus();
      return S_OK;

    case 0x0e:  /* DSP data available status / IRQ ack. 8-bit */
      m_SBDSP.ack8bitIRQ();
      *data = m_SBDSP.getRdStatus();
      return S_OK;

    case 0x0f:  /* DSP IRQ ack. 16-bit */
      m_SBDSP.ack16bitIRQ();
      *data = 0xff;
      return S_OK;

    case 0x00:
    case 0x02:
    case 0x07:  // not documented
    case 0x08:
    case 0x0b:  // not documented
    case 0x0d:  // not documented, although unofficially mentioned ("Timer Interrupt Clear") in Baresel & Jackson
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to read from unsupported port (IN 0x%3x)"), inPort));
      *data = 0xff;
      return S_OK;

    case 0x01:
    case 0x03:
    case 0x04:
    case 0x06:
    case 0x09:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to read from write-only port (IN 0x%3x)"), inPort));
      *data = 0xff;
      return S_OK;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

STDMETHODIMP CSBCompatCtl::HandleOUTB(USHORT outPort, BYTE data) {
  switch (outPort - m_basePort) {
    case 0x04:  /* MIXER register port */
      m_SBMixer.setAddress(data);
      return S_OK;

    case 0x05:  /* MIXER data register */
      m_SBMixer.setValue(data);
      return S_OK;

    case 0x06:  /* DSP reset */
      m_SBDSP.reset(data);
      return S_OK;

    case 0x0c:  /* DSP write data or command */
      m_SBDSP.putCommand(data);
      return S_OK;

    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x07:  // not documented
    case 0x08:
    case 0x09:
    case 0x0b:  // not documented
    case 0x0d:  // not documented, although unofficially mentioned ("Timer Interrupt Clear") in Baresel & Jackson
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to write to unsupported port (OUT 0x%3x, 0x%02x)"), outPort, data));
      return S_OK;

    case 0x0a:
    case 0x0e:
    case 0x0f:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to write to read-only port (OUT 0x%3x, 0x%02x)"), outPort, data));
      return S_OK;

    default:
      return E_FAIL;
  }
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CSBCompatCtl::HandleINW(USHORT inPort, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CSBCompatCtl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CSBCompatCtl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CSBCompatCtl::HandleOUTW(USHORT outPort, USHORT data) {
  return E_NOTIMPL;
}
STDMETHODIMP CSBCompatCtl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}
STDMETHODIMP CSBCompatCtl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// IMPU401HWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

void CSBCompatCtl::logError(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, (LPCTSTR)CString(message));
}

void CSBCompatCtl::logWarning(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, (LPCTSTR)CString(message));
}

