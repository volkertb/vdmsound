// SBCompatCtl.h : Declaration of the CSBCompatCtl

#ifndef __SBCOMPATCTL_H_
#define __SBCOMPATCTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IDMAHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
#import <IDMAC.tlb>
#import <IWave.tlb>
#import <IAddressable.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "SBCompatCtlDSP.h"
#include "SBCompatCtlMixer.h"

/////////////////////////////////////////////////////////////////////////////
// CSBCompatCtl
class ATL_NO_VTABLE CSBCompatCtl : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CSBCompatCtl, &CLSID_SBCompatCtl>,
  public ISBDSPHWEmulationLayer,
  public ISBMixerHWEmulationLayer,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler,
  public IDMAHandler
{
public:
  CSBCompatCtl()
    : m_SBMixer(this), m_SBDSP(this, &m_SBMixer)
    { }

DECLARE_REGISTRY_RESOURCEID(IDR_SBCOMPATCTL)
DECLARE_NOT_AGGREGATABLE(CSBCompatCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSBCompatCtl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
  COM_INTERFACE_ENTRY(IDMAHandler)
END_COM_MAP()

// ISBDSPHWEmulationLayer, ISBMixerHWEmulationLayer
public:
  void startTransfer(transfer_t type, char E2Reply, bool isSynchronous = false);
  void startTransfer(transfer_t type, int numChannels, int samplesPerSecond, int bitsPerSample, int samplesPerBlock, codec_t codec, bool isAutoInit, bool isSynchronous = false);
  void stopTransfer(transfer_t type, bool isSynchronous = false);
  void pauseTransfer(transfer_t type);
  void resumeTransfer(transfer_t type);
  void generateInterrupt(int count = 1);
  void logError(const char* message);
  void logWarning(const char* message);
  void logInformation(const char* message);

// ISupportsErrorInfo
public:
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IVDMBasicModule
public:
  STDMETHOD(Init)(IUnknown * configuration);
  STDMETHOD(Destroy)();

// IIOHandler
public:
  STDMETHOD(HandleINB)(USHORT inPort, BYTE * data);
  STDMETHOD(HandleINW)(USHORT inPort, USHORT * data);
  STDMETHOD(HandleINSB)(USHORT inPort, BYTE * data, USHORT count, DIR_T direction);
  STDMETHOD(HandleINSW)(USHORT inPort, USHORT * data, USHORT count, DIR_T direction);
  STDMETHOD(HandleOUTB)(USHORT outPort, BYTE data);
  STDMETHOD(HandleOUTW)(USHORT outPort, USHORT data);
  STDMETHOD(HandleOUTSB)(USHORT outPort, BYTE * data, USHORT count, DIR_T direction);
  STDMETHOD(HandleOUTSW)(USHORT outPort, USHORT * data, USHORT count, DIR_T direction);

// IDMAHandler
public:
  STDMETHOD(HandleTransfer)(BYTE channel, TTYPE_T type, TMODE_T mode, LONG isAutoInit, ULONG physicalAddr, ULONG maxData, LONG isDescending, ULONG * transferred, LONG * isTooSlow);
  STDMETHOD(HandleAfterTransfer)(BYTE channel, ULONG transferred, LONG isTerminalCount);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_basePort;               // base I/O port of emulation
  int m_IRQLine;                // IRQ line used by emulation to signal to DOS applications
  int m_DMA8Channel;            // 8-bit DMA channel to be used by emulation
  int m_DMA16Channel;           // 16-bit DMA channel to be used by emulation

  int m_forcedSampleRate;       // debug - sample rate to override that provided by DOS applications
  int m_forcedBitsPerSample;    // debug - number of bits/sample to override that provided by DOS applications
  int m_forcedNumChannels;      // debug - number of channels to override that provided by DOS applications

// Other member variables
protected:
  DWORD m_transferStartTime;    // time when a DMA transfer was started
  DWORD m_transferPauseTime;    // time when a DMA transfer was halted (via a DSP "pause" command)
  DWORD m_lastTransferTime;     // time when the last chunk of data was processed in the current DMA transfer
  long m_transferredBytes;      // number of bytes processed to date as part of the current DMA transfer
  long m_avgBandwidth;          // average bandwidth expected to be required by current DMA transfer based on audio parameters (sample rate, bits/sample, mono/stereo, etc.)
  long m_DSPBlockSize;          // how many bytes of data can be transferred during the current DMA transfer before an IRQ needs to be generated
  int m_bitsPerSample;          // how many bits are in an audio sample; used to differentiate between 8-bit and 16-bit audio decoding, and to align chunks of data sent to the DSP for decoding
  int m_numChannels;            // how many channels make up the audio stream; used to to align chunks of data sent to the DSP for decoding
  codec_t m_codec;
  bool m_isPaused;
  bool m_isAutoInit;
  int m_activeDMAChannel;
  transfer_t m_transferType;
  double m_renderLoad;

  BYTE m_E2Reply;

  CCriticalSection m_mutex;

// Platform-independent classes
protected:
  CSBCompatCtlMixer m_SBMixer;
  CSBCompatCtlDSP m_SBDSP;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IDMACLib::IDMAControllerPtr m_DMACtl;
  IWAVELib::IWaveDataConsumerPtr m_waveOut;
  IAddressableLib::IAddressablePtr m_AdLib;
};

#endif //__SBCOMPATCTL_H_
