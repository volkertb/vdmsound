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

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

long round(long, int);
void bufferReverse(BYTE*, int, bool);
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
      m_SBDSP.ack8BitIRQ();
      *data = m_SBDSP.getRdStatus();
      return S_OK;

    case 0x0f:  /* DSP IRQ ack. 16-bit */
      m_SBDSP.ack16BitIRQ();
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

STDMETHODIMP CSBCompatCtl::HandleTransfer(BYTE channel, TTYPE_T type, TMODE_T mode, LONG isAutoInit, ULONG physicalAddr, ULONG maxData, LONG isDescending, ULONG * transferred) {
  if (transferred == NULL)
    return E_POINTER;

  *transferred = 0;

  // If this transfer does not occur on the active DMA channel, ignore it
  if (channel != m_activeDMAChannel)
    return S_FALSE;

  // If transfer is paused, don't process any bytes
  if (m_isPaused)
    return S_OK;

  /* TODO: buffer can be as much as 4 times larger for ADPCM2 */

  int bufSize;                // how much relevant data is stored in the buffer <buf>
  BYTE buf[65536];            // temporary storage for processing (e.g. dcompressing) data 

  /* TODO: inquire: xfer 0 bytes if IRQ was not acknowledged ? */

  // Compute how many bytes should be transferred; round in such a way so that
  //   the transfer will not break a 16-bit sample down the middle, or split
  //   a left/right sample pair (for stereo data)
  long toTransfer = round(
    ((m_avgBandwidth * (LONGLONG)(timeGetTime() - m_transferStartTime)) / 1000) - m_transferredBytes,
    (m_numChannels - 1) + (m_bitsPerSample >> 4)); // rounding boundaries: 2^0 for mono 8-bit, 2^1 for 16-bit mono or 8-bit stereo, 2^2 for 16-bit stereo

  // One last detail regarding 8-bit vs. 16-bit DMA transfer (*not* audio samples)
  if (channel < 4) {          // 8-bit DMA transfer, maxData is in BYTES
    toTransfer = toTransfer > maxData ? maxData : toTransfer;
  } else {                    // 16-bit DMA transfer, maxData is in WORDS
    toTransfer = round(toTransfer, 1);  // make sure we transfer in multiples of WORDS, or else we can lose up to one byte/transfer
    toTransfer = toTransfer > (2 * maxData) ? (2 * maxData) : toTransfer;
  }

  // If no bytes need to be transferred, finish now
  if (toTransfer == 0)
    return S_OK;

  // Transfer data from application memory to the temporary buffer <buf>
  m_transferredBytes += toTransfer;

  try {
    if (isDescending) {       // is data transferred in reverse order ?
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
      bufSize = toTransfer;   // no decoding necessary; there is a 1:1 correspondence
      break;
    case CODEC_PCM_SIGNED:
      bufSize = CODEC_SignedPCM_decode(buf, toTransfer, m_bitsPerSample);
      break;
    default:
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Unsupported CODEC: %d"), (int)m_codec));
      bufSize = toTransfer;
  }

  // Play the data
  if (m_waveOut != NULL) try {
    m_waveOut->PlayData(buf, bufSize);
  } catch (_com_error& ce) {
    CString args = Format(_T("%p, %d"), buf, bufSize);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("PlayData(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
    return S_FALSE;
  }

# if _DEBUG
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transferring: after %dms, %d bytes (%d bytes in last burst) @ %dcps from %p, type = %d, mode = %d, dir = %d, A/I = %d"), (int)(timeGetTime() - m_transferStartTime), (int)m_transferredBytes, (int)toTransfer, (int)m_avgBandwidth, (int)physicalAddr, (int)type, (int)mode, (int)isDescending, (int)isAutoInit));
#endif

  if (channel < 4) {          // 8-bit DMA transfer, keep BYTE count as BYTE count
    *transferred = toTransfer;
  } else {                    // 16-bit DMA transfer, convert BYTE count into WORD count
    *transferred = toTransfer / 2;
  }

  return S_OK;
}

STDMETHODIMP CSBCompatCtl::HandleAfterTransfer(BYTE channel, ULONG transferred, LONG isTerminalCount) {
  long lastTransfer;

  // If this notification does not concern the active DMA channel, ignore it
  if (channel != m_activeDMAChannel)
    return S_FALSE;

  if (channel < 4) {          // 8-bit DMA transfer, keep BYTE count as BYTE count
    lastTransfer = transferred;
  } else {                    // 16-bit DMA transfer, convert WORD count into BYTE count
    lastTransfer = 2 * transferred;
  }

  if (lastTransfer > m_transferredBytes % m_DSPBlockSize) {
    if (m_bitsPerSample != 16) {
      m_SBDSP.set8BitIRQ();
    } else {
      m_SBDSP.set16BitIRQ();
    }

    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Interrupting (%s): after %dms, %d bytes%s"), m_bitsPerSample != 16 ? _T("8-bit") : _T("16-bit"), (int)(timeGetTime() - m_transferStartTime), (int)m_transferredBytes, isTerminalCount != 0 ? _T(" (terminal count)") : _T("")));
  }

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// ISBDSPHWEmulationLayer, ISBMixerHWEmulationLayer
/////////////////////////////////////////////////////////////////////////////

void CSBCompatCtl::startTransfer(transfer_t type, int numChannels, int samplesPerSecond, int bitsPerSample, int samplesPerBlock, codec_t codec, bool isAutoInit, bool isSynchronous) {
  // TODO: implement (recording vs. playback, codecs, etc.)

  _ASSERTE((numChannels == 1) || (numChannels == 2));
  _ASSERTE((bitsPerSample == 2) || (bitsPerSample == 3) || (bitsPerSample == 4) || (bitsPerSample == 8) || (bitsPerSample == 16));

  // Abort any active transfers
  stopTransfer(type, false);

  // Adjust sample rates to fit in acceptable interval
  if (samplesPerSecond < 4000) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Sample rate (%dHz) is inferior to the %dHz lower bound; adjusting"), samplesPerSecond, 4000));
    samplesPerSecond = 4000;
  } else if (samplesPerSecond > 44100) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Sample rate (%dHz) is superior to the %dHz upper bound; adjusting"), samplesPerSecond, 44100));
    samplesPerSecond = 44100;
  }

  // Set up the transfer state
  m_transferStartTime = timeGetTime();  // when the transfer started
  m_transferredBytes = 0;               // how many bytes were transferred to date
  m_avgBandwidth = numChannels * samplesPerSecond * bitsPerSample / 8;
  m_DSPBlockSize = samplesPerBlock * bitsPerSample / 8;
  m_bitsPerSample = bitsPerSample;      // how many bits in a sample
  m_numChannels = numChannels;          // audio channels (1 = mono, 2 = stereo)
  m_codec = codec;                      // CODEC to be used for converting DMA data
  m_isPaused = false;                   // can start transferring ASAP
  m_activeDMAChannel = (bitsPerSample != 16 ? m_DMA8Channel : m_DMA16Channel);   /* TODO: must also be able to handle 16-bit data through 8-bit DMA channels when 16-bit channels are masked out in mixer registers */

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
    m_DMACtl->InitiateTransfer(m_activeDMAChannel, isSynchronous);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_activeDMAChannel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("InitiateTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::stopTransfer(transfer_t type, bool isSynchronous) {
  try {
    m_DMACtl->AbortTransfer(m_activeDMAChannel, isSynchronous);
  } catch (_com_error& ce) {
    CString args = Format(_T("%d"), m_activeDMAChannel);
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("AbortTransfer(%s): 0x%08x - %s"), (LPCTSTR)args, ce.Error(), ce.ErrorMessage()));
  }
}

void CSBCompatCtl::pauseTransfer(transfer_t type) {
  // TODO: implement ?
  if (!m_isPaused) {
    m_transferPauseTime = timeGetTime();
    m_isPaused = true;
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transfer paused (ch. %d)"), m_activeDMAChannel));
  } else {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to pause an already paused transfer")));
  }
}

void CSBCompatCtl::resumeTransfer(transfer_t type) {
  // TODO: implement ?
  if (m_isPaused) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("DMA transfer resumed (ch. %d)"), m_activeDMAChannel));
    m_transferStartTime += (timeGetTime() - m_transferPauseTime); // adjust delta-time with inactivity amount
    m_isPaused = false;
  } else {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Attempted to resume an already active transfer")));
  }
}

void CSBCompatCtl::generateInterrupt(void) {
  try {
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
// Performs signed-PCM decoding in-place.
//
inline int CODEC_SignedPCM_decode(
    BYTE* buf,
    int bufSize,
    int bitsPerSample)
{
  int i;
  WORD* buf16;

  switch (bitsPerSample) {
    case 8: /* 8-bit quantities */
      for (i = 0; i < bufSize; i++) buf[i] += 0x80;
      return bufSize;
    case 16:
      buf16 = (WORD*)buf;
      for (i = 0; i < bufSize / 2; i++) buf16[i] += 0x8000;
      return bufSize;
    default:
      return bufSize;
  }
}
