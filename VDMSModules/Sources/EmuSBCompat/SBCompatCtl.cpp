// SBCompatCtl.cpp : Implementation of CSBCompatCtl
#include "stdafx.h"
#include "EmuSBCompat.h"
#include "SBCompatCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_DMACTL        L"DMACtl"
#define INI_STR_WAVEOUT       L"WaveOut"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_IRQLINE       L"IRQ"
#define INI_STR_DMA8CHANNEL   L"DMA8"
#define INI_STR_DMA16CHANNEL  L"DMA16"

/////////////////////////////////////////////////////////////////////////////

#define MK_SEGMENT(physicalAddr)  ((physicalAddr) >> 4)
#define MK_OFFSET(physicalAddr)   ((physicalAddr) & 0x000f)

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
    &IID_IDMAHandler
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
    Depends   = configuration;  // Dependency query object
    Config    = configuration;  // Configuration query object

    // Obtain VDM Services instance (if available)
    IUnknownPtr VDMServices
              = Depends->Get(INI_STR_VDMSERVICES);
    m_BaseSrv = VDMServices;    // Base services (registers, interrupts, etc)
    m_IOSrv   = VDMServices;    // I/O services (I/O port hooks)

    if (m_BaseSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMBaseServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);
    if (m_IOSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMIOServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

    // Obtain a DMA transfer management instance
    m_DMACtl  = Depends->Get(INI_STR_DMACTL);

    if (m_DMACtl == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_DMACTL), _T("IDMAController")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

    // Try to obtain the SB settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x220, 16, false);
    m_IRQLine  = CFG_Get(Config, INI_STR_IRQLINE, 7, 10, false);
    m_DMA8Channel  = CFG_Get(Config, INI_STR_DMA8CHANNEL, 1, 10, false);
    m_DMA16Channel = CFG_Get(Config, INI_STR_DMA16CHANNEL, 5, 10, false);

    // Try to obtain an interface to a Wave-out module, use NULL if none available
    m_waveOut  = DEP_Get(Depends, INI_STR_WAVEOUT, NULL, false);
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  IVDMSERVICESLib::IIOHandler* pIOHandler = NULL; // IIOHandler interface to <this>

  // Register the I/O handlers
  try {
    // Obtain a COM IIOHandler interface on this C++ object
    if (FAILED(hr = QueryInterface(__uuidof(IVDMSERVICESLib::IIOHandler), (void**)(&pIOHandler))))
      throw _com_error(hr);     // Failure

    // Add this object as an I/O handler on the specified port range
    m_IOSrv->AddIOHook(m_basePort, 16, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pIOHandler);

    pIOHandler->Release();      // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pIOHandler != NULL)
      pIOHandler->Release();    // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  IDMACLib::IDMAHandler* pDMAHandler = NULL;      // IDMAHandler interface to <this>

  // Register the DMA handler(s)
  try {
    // Obtain a COM IDMAHandler interface on this C++ object
    if (FAILED(hr = QueryInterface(__uuidof(IDMACLib::IDMAHandler), (void**)(&pDMAHandler))))
      throw _com_error(hr);     // Failure

    // Add this object as a handler on the specified DMA channels
    m_DMACtl->AddDMAHandler(m_DMA8Channel, pDMAHandler);
    m_DMACtl->AddDMAHandler(m_DMA16Channel, pDMAHandler);

    pDMAHandler->Release();     // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pDMAHandler != NULL)
      pDMAHandler->Release();   // Release the (unused) interface

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

  // Release the Wave-out module
  m_waveOut = NULL;

  // Release the VDM Services module
  m_IOSrv   = NULL;
  m_BaseSrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("SBCompatCtl released")));
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
// IDMAHandler
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSBCompatCtl::HandleTransfer(BYTE channel, TTYPE_T type, TMODE_T mode, LONG isAutoInit, ULONG physicalAddr, ULONG maxBytes, LONG isDescending, ULONG * transferred) {
  if (transferred == NULL)
    return E_POINTER;

  BYTE buf[65536];  /* TODO: can be as much as 4 times larger for ADPCM2 */

  /* TODO: inquire: xfer 0 bytes if IRQ was not acknowledged? */
  ULONG toTransfer = ((m_avgBandwidth * (timeGetTime() - m_transferStartTime)) / 1000) - m_transferredBytes;
  toTransfer = toTransfer > maxBytes ? maxBytes : toTransfer;
  m_transferredBytes += toTransfer;

  _ASSERTE(physicalAddr < 0x100000);  // fits in segment:offset ?

  try {
    m_BaseSrv->GetMemory(MK_SEGMENT(physicalAddr), MK_OFFSET(physicalAddr), IVDMSERVICESLib::ADDR_V86, buf, toTransfer);
  } catch (_com_error& ce) {
    CString args = Format(_T("0x%04x, 0x%04x, %d, %p, %d"), MK_SEGMENT(physicalAddr), MK_OFFSET(physicalAddr), IVDMSERVICESLib::ADDR_V86, buf, toTransfer);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("GetMemory(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }

//memcpy(buf,(const void*)(physicalAddr|0x100000),toTransfer);

  if (m_waveOut != NULL) try {
    m_waveOut->PlayData(buf, toTransfer);
  } catch (_com_error& ce) {
    CString args = Format(_T("%p, %d"), buf, toTransfer);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transferring: after %dms, %d bytes (%d bytes in last burst) @ %dcps from %p (%04x:%04x), type = %d, mode = %d, dir = %d, A/I = %d"), (int)(timeGetTime() - m_transferStartTime), (int)m_transferredBytes, (int)toTransfer, (int)m_avgBandwidth, (int)physicalAddr, (int)MK_SEGMENT(physicalAddr), (int)MK_OFFSET(physicalAddr), (int)type, (int)mode, (int)isDescending, (int)isAutoInit));

  *transferred = toTransfer;

  return S_OK;
}

STDMETHODIMP CSBCompatCtl::HandleAfterTransfer(BYTE channel, ULONG transferred, LONG isTerminalCount) {
  if (m_transferredBytes % m_DSPBlockSize < transferred) {
    generateInterrupt();
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Interrupting: after %dms, %d bytes"), (int)(timeGetTime() - m_transferStartTime), (int)m_transferredBytes));
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMPU401HWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

void CSBCompatCtl::startTransfer(transfer_t type, int numChannels, int samplesPerSecond, int bitsPerSample, int samplesPerBlock, codec_t codec, bool isAutoInit) {
  // TODO: implement
  m_transferStartTime = timeGetTime();
  m_transferredBytes = 0;
  m_avgBandwidth = numChannels * samplesPerSecond * bitsPerSample / 8;
  m_DSPBlockSize = numChannels * samplesPerBlock * bitsPerSample / 8;

  _ASSERTE(m_avgBandwidth > 0);

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Transferring (%s) %s, %dHz, %dbps, %d samples, %s"), type == TT_PLAYBACK ? _T("playback") : type == TT_RECORD ? _T("record") : _T("<unknown>"), numChannels == 1 ? _T("mono") : numChannels == 2 ? _T("stereo") : _T("<unknown>"), samplesPerSecond, bitsPerSample, samplesPerBlock, isAutoInit ? _T("auto-init") : _T("single-cycle")));

  if (m_waveOut != NULL) try {
    m_waveOut->SetFormat(numChannels, samplesPerSecond, bitsPerSample); /* TODO: decide how to treat/decompress ADPCM: as 8 or as 16-bit? */
  } catch (_com_error& ce) {
    CString args = Format(_T("%d, %d, %d"), numChannels, samplesPerSecond, bitsPerSample);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }

  try {
    m_DMACtl->InitiateTransfer(m_DMA8Channel);  /* TODO: select channel based on mixer's DMA channel mask */
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_DMA8Channel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("InitiateTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::pauseTransfer(transfer_t type) {
  // TODO: implement
}

void CSBCompatCtl::resumeTransfer(transfer_t type) {
  // TODO: implement
}

void CSBCompatCtl::stopTransfer(transfer_t type) {
  // TODO: implement
}

void CSBCompatCtl::generateInterrupt(void) {
  // TODO: implement
  try {
    /* TODO: set interrupt "mask" in mixer */
    m_BaseSrv->SimulateInterrupt(IVDMSERVICESLib::INT_MASTER, m_IRQLine, 1);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d, %d, %d"), IVDMSERVICESLib::INT_MASTER, m_IRQLine, 1);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SimulateInterrupt(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::logError(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, (LPCTSTR)CString(message));
}

void CSBCompatCtl::logWarning(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, (LPCTSTR)CString(message));
}

void CSBCompatCtl::logInformation(const char* message) {
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, (LPCTSTR)CString(message));
}
