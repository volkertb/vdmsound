// AdLibCtl.h : Declaration of the CAdLibCtl

#ifndef __ADLIBCTL_H_
#define __ADLIBCTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

namespace MAME {
# include "fmopl.h"
}

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
#import <IWave.tlb>

/////////////////////////////////////////////////////////////////////////////

#define MAX_INSTANCES 16

/////////////////////////////////////////////////////////////////////////////
// CAdLibCtl
class ATL_NO_VTABLE CAdLibCtl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CAdLibCtl, &CLSID_AdLibCtl>,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler
{
public:
	CAdLibCtl()
    : m_instanceID(-1), m_OPL(NULL)
    { }

DECLARE_REGISTRY_RESOURCEID(IDR_ADLIBCTL)
DECLARE_NOT_AGGREGATABLE(CAdLibCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAdLibCtl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
END_COM_MAP()

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
  static void OPLTimerHandler(int channel, double interval_sec);
  static void OPLUpdateHandler(int param, int min_interval_usec);

protected:
  HRESULT OPLCreate(int sampleRate);
  void OPLDestroy(void);

/////////////////////////////////////////////////////////////////////////////

// Types
protected:
  struct TimerInfo {
    bool isActive;
    DWORD expiration;
  };

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_basePort;
  int m_sampleRate;

// Platform-independent classes
protected:
  MAME::FM_OPL* m_OPL;

// Other member variables
protected:
  static bool isInstancesInitialized;
  static CAdLibCtl* instances[MAX_INSTANCES];
  static CCriticalSection OPLMutex;

  int m_instanceID;

  TimerInfo m_targetTime[2];

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IWAVELib::IWaveDataConsumerPtr m_waveOut;
};

#endif //__ADLIBCTL_H_
