// PPDACCtl.h : Declaration of the CPPDACCtl

#ifndef __PPDACCTL_H_
#define __PPDACCTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
#import <IWave.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////
// CPPDACCtl
class ATL_NO_VTABLE CPPDACCtl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CPPDACCtl, &CLSID_PPDACCtl>,
  public IRunnable,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler
{
public:
	CPPDACCtl()
    : m_lastTime(0), m_curTime(0), m_bufPtr(0)
    { }

DECLARE_REGISTRY_RESOURCEID(IDR_PPDACCTL)
DECLARE_NOT_AGGREGATABLE(CPPDACCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPPDACCtl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
END_COM_MAP()

// IRunnable
public:
  unsigned int Run(CThread& thread);

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

protected:
  __int64 getTimeMicros(void);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_basePort;
  int m_sampleRate;

// Other member variables
protected:
  CThread m_playbackThread;

  CCriticalSection m_lock;
  volatile BYTE m_buffer[65536];
  volatile int m_bufPtr;

  __int64 m_lastTime, m_curTime;
  volatile double m_renderLoad;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IWAVELib::IWaveDataConsumerPtr m_waveOut;
};

#endif //__PPDACCTL_H_
