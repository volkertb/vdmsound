// SBCompatCtl.cpp : Implementation of CSBCompatCtl
#include "stdafx.h"
#include "EmuSBCompat.h"
#include "SBCompatCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE   _T("The dependency module '%1' does not support the '%2' interface.%0")
#define MSG_ERR_FMT_VERSION _T("The version string '%1' is not properly formatted.  Please use a value of the form X.Y, where X is the major and Y is the minor version.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"
#define INI_STR_DMACTL        L"DMACtl"
#define INI_STR_WAVEOUT       L"WaveOut"

#define INI_STR_DSPVERSION    L"version"
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

long round(long, int);
void bufferReverse(BYTE*, int, bool);
int CODEC_UnsignedPCM_decode(BYTE*, int, int);
int CODEC_SignedPCM_decode(BYTE*, int, int);

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



#define _XYZ(str) fprintf(xyz,(LPCSTR)str);
FILE* xyz=NULL;

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

    // Try to obtain the SB version to emulate (affects small details on how the DSP behaves)
    int vMajor, vMinor;
    _bstr_t versionStr = CFG_Get(Config, INI_STR_DSPVERSION, L"4.05", false);

    if (sscanf((LPCSTR)versionStr, "%d.%d", &vMajor, &vMinor) != 2)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_FMT_VERSION, /*false, NULL, 0, */false, (LPCTSTR)versionStr), __uuidof(IVDMBasicModule), E_ABORT);

    m_DSPVersion = MAKEWORD((BYTE)vMinor, (BYTE)vMajor);

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

    m_activeDMAChannel = m_DMA8Channel;

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

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("SBCompatCtl initialized (DSP version = %d.%02d, base port = 0x%03x, IRQ = %d, 8-bit DMA = %d, 16-bit DMA = %d)"), HIBYTE(m_DSPVersion) & 0xff, LOBYTE(m_DSPVersion) & 0xff, m_basePort, m_IRQLine, m_DMA8Channel, m_DMA16Channel));
//xyz=fopen("D:\\vdmsdma.txt","wt");
  return S_OK;
}

STDMETHODIMP CSBCompatCtl::Destroy() {
  // put the mixer in a known state
  m_SBMixer.reset();

if(xyz)fclose(xyz);

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

# ifdef _DEBUG
  try { RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("%04x:%08lx INB %03x"), m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_CS) & 0xffff, m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_EIP) & 0xffffffffl, (int)inPort)); } catch (...) { }
# endif

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
      m_SBDSP.ack8BitIRQ();
      *data = m_SBDSP.getRdStatus();
      return S_OK;

    case 0x0f:  /* DSP IRQ ack. 16-bit */
      m_SBDSP.ack16BitIRQ();
      *data = 0xff;
      return S_OK;

    case 0x00:
    case 0x02:
    case 0x08:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to read from unsupported FM port (IN 0x%3x)"), inPort));
      *data = *data;       // Do not modify AL; this can help in passing some DOS games' tests (e.g. Day of the Tentacle)
      return S_FALSE;

    case 0x07:  // not documented
    case 0x0b:  // not documented
    case 0x0d:  // not documented, although unofficially mentioned ("Timer Interrupt Clear") in Baresel & Jackson
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to read from undocumented port (IN 0x%3x)"), inPort));
      *data = 0xff;
      return S_FALSE;

    case 0x01:
    case 0x03:
    case 0x04:
    case 0x06:
    case 0x09:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to read from write-only port (IN 0x%3x)"), inPort));
      *data = 0xff;
      return S_FALSE;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

STDMETHODIMP CSBCompatCtl::HandleOUTB(USHORT outPort, BYTE data) {

# ifdef _DEBUG
  try { RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("%04x:%08lx OUTB %03x, %02x"), m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_CS) & 0xffff, m_BaseSrv->GetRegister(IVDMSERVICESLib::REG_EIP) & 0xffffffffl, (int)outPort, data & 0xff)); } catch (...) { }
# endif

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
    case 0x08:
    case 0x09:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to write to unsupported FM port (OUT 0x%3x, 0x%02x)"), outPort, data));
      return S_FALSE;

    case 0x07:  // not documented
    case 0x0b:  // not documented
    case 0x0d:  // not documented, although unofficially mentioned ("Timer Interrupt Clear") in Baresel & Jackson
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Attempted to write to undocumented port (OUT 0x%3x, 0x%02x)"), outPort, data));
      return S_FALSE;

    case 0x0a:
    case 0x0e:
    case 0x0f:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to write to read-only port (OUT 0x%3x, 0x%02x)"), outPort, data));
      return S_FALSE;

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

STDMETHODIMP CSBCompatCtl::HandleTransfer(BYTE channel, TTYPE_T type, TMODE_T mode, LONG isAutoInit, ULONG physicalAddr, ULONG maxData, LONG isDescending, ULONG * transferred, LONG * isTooSlow) {
  if (transferred == NULL)
    return E_POINTER;

  *transferred = 0;
  *isTooSlow = false;

_XYZ("*");

  // Lock all transfer variables (DMA ch., sample rate, etc.)
  CSingleLock lock(&m_mutex, TRUE);
_XYZ("# ");

  // If this transfer does not occur on the active DMA channel, ignore it
  if (channel != m_activeDMAChannel)
    return S_FALSE;

_XYZ("1");
  // If we are dealing with a DSP command 0xe2 1-byte transfer, do it now
  if (m_transferType == TT_E2CMD) {
    // No matter what, transfer 1 byte (done on an 8-bit channel)
    *transferred = 1;
    m_transferredBytes += *transferred;

    // Verify that DMA transfer type and expected transfer type match
    if (type != TRT_WRITE) {
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("HandleTransfer: DMA transfer type mismatch, expected WRITE for 0xe2 DSP command, but found %s (%d) instead"), type == TRT_VERIFY ? _T("VERIFY") : type == TRT_READ ? _T("READ") : type == TRT_INVALID ? _T("INVALID") : _T("<unknown>"), (int)type));
      return S_FALSE;
    } else {
      try {
        m_BaseSrv->SetMemory(0, physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, &m_E2Reply, *transferred);
      } catch (_com_error& ce) {
        CString args = Format(_T("0x%04x, 0x%04x, %d, %p, %d"), 0, physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, &m_E2Reply, *transferred);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("GetMemory(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
        return S_FALSE;
      }

#     if _DEBUG
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transferring: after %dms, %d bytes (%d bytes in last burst) to/from %p, type = %d, mode = %d, dir = %d, A/I = %d"), (int)(timeGetTime() - m_lastTransferTime), (int)m_transferredBytes, (int)(*transferred), (int)physicalAddr, (int)type, (int)mode, (int)isDescending, (int)isAutoInit));
#     endif

      return S_OK;
    }
  }

_XYZ("2");
  // If transfer is paused, don't process any bytes
  if (m_isPaused)
    return S_OK;

_XYZ("3");
  // On SB16: if the last IRQ was not acknowledged, don't process any bytes
  if ((getDSPVersion() >= 0x0400) &&
      (m_bitsPerSample != 16 ? m_SBDSP.get8BitIRQ() : m_SBDSP.get16BitIRQ()))
  {
    return S_OK;
  }

_XYZ("4");
  // Get the current time
  DWORD currentTime = timeGetTime();
  DWORD deltaTime = currentTime - m_lastTransferTime;
  m_lastTransferTime = currentTime;

  // Determine the data alignment (BYTE for 8-bit channels, WORD for 16-bit channels)
  int alignment = ((channel < 4) ? 0 : 1);  // 0 = BYTE (2^0 bytes alignment), 1 = WORD (2^1 alignment)

  // Performance 'hack' (quick-DMA)

  // Many games verify the SB by performing a 1- or 4-bytes transfer; some games' detection routine times
  // out very quickly (20-50ms), so if we are dealing with such a transfer:
  // 1) don't even bother playing back the data (it's garbage anyway, usually comes from 0000:0000)
  // 2) return ASAP, pretending everything was transmitted (prevents the game's detection from time-out'ing)

  if ((m_transferredBytes + (maxData << alignment)) < 32) {
    // Pretend to transfer everything, and do it in one shot
    long toTransfer = maxData << alignment; // maxData is BYTES for 8-bit DMA, WORDS for 16-bit DMA -- convert to BYTES
    m_transferredBytes += toTransfer;

#   if _DEBUG
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transferring: after %dms, %d bytes (%d bytes in last burst) @ %dcps to/from %p, type = %d, mode = %d, dir = %d, A/I = %d (quick-DMA)"), (int)(deltaTime), (int)m_transferredBytes, (int)toTransfer, (int)m_avgBandwidth, (int)physicalAddr, (int)type, (int)mode, (int)isDescending, (int)isAutoInit));
#   endif

    *transferred = maxData;
    return S_OK;
  }

_XYZ("5");
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  /* TODO: buffer can be as much as 4 times larger for ADPCM2 playback ! */

  int bufSize;                // how much relevant data is stored in the buffer <buf>
  BYTE buf[65536];            // temporary storage for processing (e.g. dcompressing) data 

  // Compute by how much this transfer should be boosted or diminished, based on
  //   playback performance (feedback indicating playback buffer overrun/underrun)
  double scalingFactor = min(2.0, max(0.0, 1 / m_renderLoad));

  // Compute the maximum amount of bytes we can exchange in this transaction
  long maxTransfer = min(
      m_DSPBlockSize - (m_transferredBytes % m_DSPBlockSize), // SB block limit
      maxData << alignment);                                  // DMA terminal count limit

  // Compute how many bytes should be transferred based on average bandwidth, on
  //   the time elapsed since the last transfer (+/- 1 time uncertainty), and on
  //   the scaling factor computed above.
  long toTransfer = scalingFactor * max(0.0, (m_avgBandwidth * (double)(deltaTime + 1)) / 1000.0);

  // Round (down) the number of bytes to transfer in such a way so that
  //   the transfer will not break a 16-bit sample down the middle, split a
  //   left/right sample pair (for stereo data), or be misaligned on a 16-bit
  //   boundary when transferring across 16-bit DMA channels
  toTransfer = round(toTransfer, max((m_numChannels - 1) + (m_bitsPerSample >> 4), alignment)); // rounding boundaries: 2^0 for mono 8-bit over 8-bit DMA channels, 2^1 for 16-bit mono or 8-bit stereo over any DMA channels, <OR> 8-bit mono over 16-bit DMA channels, 2^2 for 16-bit stereo (any DMA channels)

_XYZ((bstr_t)Format(_T(" % 6lu % 6lu % 6lu % 4d %0.3f    % 6ld %c\n"), m_transferredBytes, toTransfer, maxTransfer, (int)deltaTime, m_renderLoad, toTransfer + (toTransfer - maxTransfer), toTransfer + (toTransfer - maxTransfer) > m_DSPBlockSize ? '^' : ' '));

  // Clip the amount of bytes to transfer if it exceeds the transfer limit
  if (toTransfer > maxTransfer) {
    // Decide whether we are doing alright, or if we need to boost DMA performance;
    // To do this, see how much we have to sacrifice in this particular transfer in
    //   order not to violate the maxTransfer limit (toTransfer - maxTransfer), and
    //   see if that amount can be in turn recuperated during the next transfer
    //   (assuming toTransfer and maxTransfer would remain pretty much the same),
    //   still without violating any constraints.
    if (toTransfer + (toTransfer - maxTransfer) > m_DSPBlockSize) {
      // What we sacrificed in this transfer cannot be recuperated during the next
      //   transfer, so request a boost.
      *isTooSlow = true;      // request an increase in DMA servicing frequency
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, _T("DMA updates too infrequent (unable to keep up with desired transfer rate), requesting boost"));
    }

    // Now clip the transfer size, and we're done
    toTransfer = maxTransfer;
  } else {
    // Occasionally round up the value so that we don't end up with ridiculously
    //   small amounts of data to transfer, after clipping, in the next transaction
    if ((maxTransfer - toTransfer) < 128) {
      toTransfer = maxTransfer;
    }
  }

  /* TODO: put a limit on the frequency of "need to boost DMA" warnings; logging
     them incurs up to 20ms (!) or more overhead, which is the last thing we need when
     struggling for 1-2ms in an attempt to boost DMA performance */

  // If, after all calculations, no bytes need to be transferred, finish now
  if (toTransfer == 0)
    return S_OK;

  // Total amount of data transferred
  m_transferredBytes += toTransfer;

  switch (m_transferType) {
    case TT_PLAYBACK:
      // First verify that DMA transfer type and expected transfer type match
      if (type != TRT_READ) {
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("HandleTransfer: DMA transfer type mismatch, expected READ for playback, but found %s (%d) instead"), type == TRT_VERIFY ? _T("VERIFY") : type == TRT_WRITE ? _T("WRITE") : type == TRT_INVALID ? _T("INVALID") : _T("<unknown>"), (int)type));
        break;  // although this is an exceptional situation, continue and report the bytes as transferred (instead of aborting and transferring 0 bytes)
      }

      // Retrieve the data from memory
      try {
        if (isDescending) {   // is data transferred in reverse order ?
          m_BaseSrv->GetMemory(0, physicalAddr - toTransfer + 1, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
          bufferReverse(buf, toTransfer, channel < 4);  // re-arrange (swap) data so that it is ordered normally
        } else {
          m_BaseSrv->GetMemory(0, physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
        }
      } catch (_com_error& ce) {
        CString args = Format(_T("0x%04x, 0x%04x, %d, %p, %d"), 0, isDescending ? physicalAddr - toTransfer + 1 : physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("GetMemory(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
        return S_FALSE;
      }

      // Decode/decompress the data (if necessary)
      switch (m_codec) {
        case CODEC_PCM:
          bufSize = CODEC_UnsignedPCM_decode(buf, toTransfer, m_bitsPerSample);
          break;
        case CODEC_PCM_SIGNED:
          bufSize = CODEC_SignedPCM_decode(buf, toTransfer, m_bitsPerSample);
          break;
        default:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Unsupported CODEC: %d"), (int)m_codec));
          bufSize = toTransfer;
      }

      // Play the data, and update the load factor
      if (m_waveOut != NULL) try {
        m_renderLoad = m_waveOut->PlayData(buf, bufSize);
      } catch (_com_error& ce) {
        CString args = Format(_T("%p, %d"), buf, bufSize);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
        return S_FALSE;
      } break;

    case TT_RECORD:
      _ASSERTE(m_codec == CODEC_PCM);

      // First verify that DMA transfer type and expected transfer type match
      if (type != TRT_WRITE) {
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("HandleTransfer: DMA transfer type mismatch, expected WRITE for recording, but found %s (%d) instead"), type == TRT_VERIFY ? _T("VERIFY") : type == TRT_READ ? _T("READ") : type == TRT_INVALID ? _T("INVALID") : _T("<unknown>"), (int)type));
        break;  // although this is an exceptional situation, continue and report the bytes as transferred (instead of aborting and transferring 0 bytes)
      }

      /* TODO: grab data from actual wave-in */

      // Record the data (nice sawtooth here)
      for (bufSize = 0; bufSize < toTransfer; bufSize++) buf[bufSize] = bufSize % 256;

      // Put the data into memory
      try {
        if (isDescending) {   // is data transferred in reverse order ?
          bufferReverse(buf, toTransfer, channel < 4);  // re-arrange (swap) data so that it is ordered normally
          m_BaseSrv->SetMemory(0, physicalAddr - toTransfer + 1, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
        } else {
          m_BaseSrv->SetMemory(0, physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
        }
      } catch (_com_error& ce) {
        CString args = Format(_T("0x%04x, 0x%04x, %d, %p, %d"), 0, isDescending ? physicalAddr - toTransfer + 1 : physicalAddr, IVDMSERVICESLib::ADDR_PHYSICAL, buf, toTransfer);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetMemory(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
        return S_FALSE;
      } break;

    case TT_E2CMD:
      ASSERT(FALSE);  // should never get here
      return S_FALSE;

    default:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("HandleTransfer: Internal state error -- unknown transfer type (%d)"), m_transferType));
      return S_FALSE;
  }

# if _DEBUG
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transferring: after %dms, %d bytes (%d bytes in last burst) @ %dcps to/from %p, type = %d, mode = %d, dir = %d, A/I = %d; load factor = %0.5f"), (int)(deltaTime), (int)m_transferredBytes, (int)toTransfer, (int)m_avgBandwidth, (int)physicalAddr, (int)type, (int)mode, (int)isDescending, (int)isAutoInit, (float)m_renderLoad));
# endif

  // Release the lock
  lock.Unlock();

  // Update DMA state upon return
  if (channel < 4) {          // 8-bit DMA transfer, keep BYTE count as BYTE count
    *transferred = toTransfer;
  } else {                    // 16-bit DMA transfer, convert BYTE count into WORD count
    *transferred = toTransfer / 2;
  }

  return S_OK;
}

STDMETHODIMP CSBCompatCtl::HandleAfterTransfer(BYTE channel, ULONG transferred, LONG isTerminalCount) {
  long lastTransfer, overShoot;

_XYZ("$");

  // Lock all transfer variables (DMA ch., sample rate, etc.)
  CSingleLock lock(&m_mutex, TRUE);
_XYZ("%");

  // If this notification does not concern the active DMA channel, ignore it
  if (channel != m_activeDMAChannel)
    return S_FALSE;

_XYZ("A");

  switch (m_transferType) {
    case TT_PLAYBACK:
    case TT_RECORD:
      if (channel < 4) {      // 8-bit DMA transfer, keep BYTE count as BYTE count
        lastTransfer = transferred;
      } else {                // 16-bit DMA transfer, convert WORD count into BYTE count
        lastTransfer = 2 * transferred;
      }

      overShoot = m_transferredBytes % m_DSPBlockSize;  // how many bytes were transferred above the point where an IRQ should be generated

      if (lastTransfer > overShoot) {   // we passed one or more IRQ generation points during the last transfer
        int numInterrupts = ((lastTransfer - overShoot - 1) / m_DSPBlockSize) + 1;

        _ASSERTE(numInterrupts <= 1);

        if (m_bitsPerSample != 16) {
          m_SBDSP.set8BitIRQ();
        } else {
          m_SBDSP.set16BitIRQ();
        }

        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Interrupting (%s, x%d): after %dms, %d bytes%s"), m_bitsPerSample != 16 ? _T("8-bit") : _T("16-bit"), numInterrupts, (int)(timeGetTime() - m_transferStartTime), (int)m_transferredBytes, isTerminalCount != 0 ? _T(" (terminal count)") : _T("")));
      }

_XYZ("D");
      return S_OK;

    case TT_E2CMD:
      // We transferred what we had to transfer, now clean up
_XYZ("B");
      try {
        m_DMACtl->StopTransfer(m_activeDMAChannel, false); // *must* be asynchronous, otherwise we deadlock
_XYZ("C");
      } catch (_com_error& ce) {
        CString args = Format(_T("%d"), m_activeDMAChannel);
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("AbortTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
        return S_FALSE;
      } return S_OK;

    default:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("HandleAfterTransfer: Internal state error -- unknown transfer type (%d)"), m_transferType));
      return S_FALSE;
  }
}



/////////////////////////////////////////////////////////////////////////////
// ISBDSPHWEmulationLayer, ISBMixerHWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

short CSBCompatCtl::getDSPVersion(void) {
  return m_DSPVersion;
}

void CSBCompatCtl::startTransfer(transfer_t type, char E2Reply, bool isSynchronous) {
  _ASSERTE(type == TT_E2CMD);

  // Abort any active transfers
  stopTransfer(type, true);

  // Lock all transfer variables (DMA ch., sample rate, etc.)
  m_mutex.Lock(250);

  // Set up the transfer state
  m_transferStartTime = timeGetTime();  // when the transfer started
  m_lastTransferTime = m_transferStartTime;
  m_transferredBytes = 0;               // how many bytes were transferred to date

  m_transferType     = type;
  m_activeDMAChannel = m_DMA8Channel;
  m_E2Reply          = E2Reply;

  // Release the lock
  m_mutex.Unlock();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Starting DMA transfer (DSP command 0xe2) on ch. %d"), m_activeDMAChannel));

  // Start the DMA transfer
  try {
    m_DMACtl->StartTransfer(m_activeDMAChannel, isSynchronous);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_activeDMAChannel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("InitiateTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::startTransfer(transfer_t type, int numChannels, int samplesPerSecond, int bitsPerSample, int samplesPerBlock, codec_t codec, bool isAutoInit, bool isSynchronous) {
  _ASSERTE((type == TT_PLAYBACK) || (type == TT_RECORD));
  _ASSERTE((numChannels == 1) || (numChannels == 2));
  _ASSERTE((bitsPerSample == 2) || (bitsPerSample == 3) || (bitsPerSample == 4) || (bitsPerSample == 8) || (bitsPerSample == 16));

  // Abort any active transfers
  stopTransfer(type, true);

  // Adjust sample rates to fit in acceptable interval
  if (samplesPerSecond < 4000) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Sample rate (%dHz) is inferior to the %dHz lower bound; adjusting"), samplesPerSecond, 4000));
    samplesPerSecond = 4000;
  } else if (samplesPerSecond > 44100) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Sample rate (%dHz) is superior to the %dHz upper bound; adjusting"), samplesPerSecond, 44100));
    samplesPerSecond = 44100;
  }

  // Lock all transfer variables (DMA ch., sample rate, etc.)
  m_mutex.Lock(250);

  // Set up the transfer state
  m_transferStartTime = timeGetTime();  // when the transfer started
  m_lastTransferTime = m_transferStartTime;
  m_transferredBytes = 0;               // how many bytes were transferred to date
  m_avgBandwidth = numChannels * samplesPerSecond * bitsPerSample / 8;
  m_DSPBlockSize = samplesPerBlock * bitsPerSample / 8;
  m_bitsPerSample = bitsPerSample;      // how many bits in a sample
  m_numChannels = numChannels;          // audio channels (1 = mono, 2 = stereo)
  m_codec = codec;                      // CODEC to be used for converting DMA data
  m_isPaused = false;                   // can start transferring ASAP

  m_transferType = type;
  m_renderLoad = 1.00;

  if (bitsPerSample != 16) {
    m_SBDSP.ack8BitIRQ();               // clear any pending IRQs
    m_activeDMAChannel = m_DMA8Channel;
  } else {
    m_SBDSP.ack16BitIRQ();              // clear any pending IRQs
    /* TODO: must also be able to handle 16-bit data through 8-bit DMA channels when 16-bit channels are masked out in mixer registers (16-bit aliasing) */
    m_activeDMAChannel = m_DMA16Channel;
  }

  // Release the lock
  m_mutex.Unlock();

_XYZ((bstr_t)Format(_T("Starting DMA transfer (%s) on ch. %d (%s, %d samples/block): %d-bit %s %dHz, %s\n"), type == TT_PLAYBACK ? _T("playback") : type == TT_RECORD ? _T("record") : _T("<unknown transfer type>"), m_activeDMAChannel, isAutoInit ? _T("auto-init") : _T("single-cycle"), samplesPerBlock, bitsPerSample, numChannels == 1 ? _T("mono") : numChannels == 2 ? _T("stereo") : _T("<unknown aurality>"), samplesPerSecond, codec == CODEC_PCM ? _T("unsigned PCM") : codec == CODEC_PCM_SIGNED ? _T("signed PCM") : codec == CODEC_ADPCM_2 ? _T("ADPCM 2") : codec == CODEC_ADPCM_3 ? _T("ADPCM 3") : codec == CODEC_ADPCM_4 ? _T("ADPCM 4") : _T("<unknown codec>")));

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Starting DMA transfer (%s) on ch. %d (%s, %d samples/block): %d-bit %s %dHz, %s"), type == TT_PLAYBACK ? _T("playback") : type == TT_RECORD ? _T("record") : _T("<unknown transfer type>"), m_activeDMAChannel, isAutoInit ? _T("auto-init") : _T("single-cycle"), samplesPerBlock, bitsPerSample, numChannels == 1 ? _T("mono") : numChannels == 2 ? _T("stereo") : _T("<unknown aurality>"), samplesPerSecond, codec == CODEC_PCM ? _T("unsigned PCM") : codec == CODEC_PCM_SIGNED ? _T("signed PCM") : codec == CODEC_ADPCM_2 ? _T("ADPCM 2") : codec == CODEC_ADPCM_3 ? _T("ADPCM 3") : codec == CODEC_ADPCM_4 ? _T("ADPCM 4") : _T("<unknown codec>")));

  // Set up the renderer (if any)
  if (m_waveOut != NULL) try {
    m_waveOut->SetFormat(numChannels, samplesPerSecond, bitsPerSample); /* TODO: decide how to treat/decompress ADPCM: as 8 or as 16-bit? */
  } catch (_com_error& ce) {
    CString args = Format(_T("%d, %d, %d"), numChannels, samplesPerSecond, bitsPerSample);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }

  // Start the DMA transfer
  try {
    m_DMACtl->StartTransfer(m_activeDMAChannel, isSynchronous);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_activeDMAChannel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("InitiateTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

/* TODO: should wait for the current block to finish, generate the interript,
   and only then stop the DMA transaction */
void CSBCompatCtl::stopTransfer(transfer_t type, bool isSynchronous) {
  try {
    m_DMACtl->StopTransfer(m_activeDMAChannel, isSynchronous);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_activeDMAChannel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("AbortTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::pauseTransfer(transfer_t type) {
  if (!m_isPaused) {
    m_transferPauseTime = timeGetTime();
    m_isPaused = true;
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transfer paused (ch. %d)"), m_activeDMAChannel));
  } else {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to pause an already paused transfer")));
  }
}

void CSBCompatCtl::resumeTransfer(transfer_t type) {
  if (m_isPaused) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transfer resumed (ch. %d)"), m_activeDMAChannel));
    DWORD deltaTime = timeGetTime() - m_transferPauseTime;
    m_lastTransferTime  += deltaTime;
    m_transferStartTime += deltaTime;
    m_isPaused = false;
  } else {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to resume an already active transfer")));
  }
}

void CSBCompatCtl::generateInterrupt(int count) {
  try {
    m_BaseSrv->SimulateInterrupt(IVDMSERVICESLib::INT_MASTER, m_IRQLine, count);
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



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
// Rounds up to the nearest multiple of 2^pow2
//
inline long round(
    long what,
    int pow2)
{
  what >>= pow2;
  what <<= pow2;
  return what;
}

//
// Inverses a buffer by swapping its elements so that the first becomes last,
// the second becomes econd to last, etc.  Elements can be words or bytes.
//
inline void bufferReverse(
    BYTE* buf,
    int bufSize,
    bool is8bitGranularity)
{
  int i, j;
  WORD* buf16;

  if (is8bitGranularity) {  // invert BYTES
    for (i = 0, j = bufSize - 1; i <= bufSize / 2; i++, j--)
      std::swap(buf[i], buf[j]);
  } else {                  // invert WORDS
    buf16 = (WORD*)buf;
    for (i = 0, j = (bufSize / 2) - 1; i <= bufSize / 4; i++, j--)
      std::swap(buf16[i], buf16[j]);
  }
}

//
// Performs unsigned-PCM decoding in-place.
//
inline int CODEC_UnsignedPCM_decode(
    BYTE* buf,
    int bufSize,
    int bitsPerSample)
{
  int i;
  WORD* buf16;

  switch (bitsPerSample) {
    case 8:   /* 8-bit quantities */
      return bufSize;
    case 16:  /* 16-bit quantities */
      buf16 = (WORD*)buf;
      for (i = 0; i < bufSize / 2; i++) buf16[i] ^= 0x8000;
      return bufSize;
    default:
      return bufSize;
  }
}

//
// Performs signed-PCM decoding in-place.
//
inline int CODEC_SignedPCM_decode(
    BYTE* buf,
    int bufSize,
    int bitsPerSample)
{
  int i;

  switch (bitsPerSample) {
    case 8:   /* 8-bit quantities */
      for (i = 0; i < bufSize; i++) buf[i] ^= 0x80;
      return bufSize;
    case 16:  /* 16-bit quantities */
      return bufSize;
    default:
      return bufSize;
  }
}
