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
  short getDSPVersion(void);
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

protected:
  CSBCompatCtlMixer m_SBMixer;
  CSBCompatCtlDSP m_SBDSP;
  short m_DSPVersion;
  int m_basePort;
  int m_IRQLine;
  int m_DMA8Channel;
  int m_DMA16Channel;

protected:
  DWORD m_lastTransferTime;
  DWORD m_transferStartTime;
  DWORD m_transferPauseTime;
  long m_transferredBytes;
  long m_avgBandwidth;
  long m_DSPBlockSize;
  int m_bitsPerSample;
  int m_numChannels;
  codec_t m_codec;
  bool m_isPaused;
  int m_activeDMAChannel;
  transfer_t m_transferType;
  double m_renderLoad;

  BYTE m_E2Reply;

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IDMACLib::IDMAControllerPtr m_DMACtl;
  IWAVELib::IWaveDataConsumerPtr m_waveOut;
};

#endif //__SBCOMPATCTL_H_
