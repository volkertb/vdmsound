// AdLibCtl.h : Declaration of the CAdLibCtl

#ifndef __ADLIBCTL_H_
#define __ADLIBCTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

namespace MAME {
# define HAS_YM3812 1
# include "fmopl.h"
# undef __FMOPL_H_    // temp. hack
# define HAS_YMF262 1
# include "ymf262.h"
# define __FMOPL_H_   // temp. hack
}

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IAddressable.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
#import <IWave.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "AdLibCtlFSM.h"
#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////

#define MAX_INSTANCES 16
#define OPL_QUEUE_LEN 1024

/////////////////////////////////////////////////////////////////////////////
// TS_Queue

// This implements a circular queue.  This queue is thread-safe as long as
//  there is only one producer and only one consumer (faster than the
//  generic case, in which one can have any number of producers/consumers).
template<class _T,int _n> class TS_Queue {
  public:
    TS_Queue(void)
      : m_head(0), m_tail(0), m_length(0)
    { }

    bool put(const _T& element) {
      if (m_length < _n) {
        m_data[m_head] = element;
        m_head = (m_head + 1) % _n;
        InterlockedIncrement(&m_length);
        return true;
      } else {
        return false;
      }
    }

    bool get(_T& element) {
      if (m_length > 0) {
        element = m_data[m_tail];
        m_tail = (m_tail + 1) % _n;
        InterlockedDecrement(&m_length);
        return true;
      } else {
        return false;
      }
    }

  protected:
    _T m_data[_n];
    LONG m_head, m_tail, m_length;
};

/////////////////////////////////////////////////////////////////////////////
// CAdLibCtl
class ATL_NO_VTABLE CAdLibCtl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CAdLibCtl, &CLSID_AdLibCtl>,
  public IAdLibHWEmulationLayer,
  public IRunnable,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler,
  public IAddressable
{
public:
	CAdLibCtl()
    : m_instanceID(-1), m_AdLibFSM(this)
    { }

DECLARE_REGISTRY_RESOURCEID(IDR_ADLIBCTL)
DECLARE_NOT_AGGREGATABLE(CAdLibCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAdLibCtl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
  COM_INTERFACE_ENTRY(IAddressable)
END_COM_MAP()

// IAdLibHWEmulationLayer
public:
  void setOPLReg(int address, int value);
  OPLTime_t getTimeMicros(void);
  void logError(const char* message);
  void logWarning(const char* message);
  void logInformation(const char* message);

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

// IAddressable
public:
  STDMETHOD(HandleByteRead)(ULONG address, BYTE * data);
  STDMETHOD(HandleWordRead)(ULONG address, USHORT * data);
  STDMETHOD(HandleByteWrite)(ULONG address, BYTE data);
  STDMETHOD(HandleWordWrite)(ULONG address, USHORT data);

protected:
  static void OPLTimerHandler(int channel, double interval_sec);
  static void OPLUpdateHandler(int param, int min_interval_usec);

protected:
  HRESULT OPLCreate(int sampleRate);
  void OPLDestroy(void);
  void OPLPlay(DWORD deltaTime);
  HRESULT OPLRead(BYTE address, BYTE * data);
  HRESULT OPLWrite(BYTE address, BYTE data);

/////////////////////////////////////////////////////////////////////////////

// Types
protected:
  struct OPLMessage {
    DWORD timestamp;
    int address;
    int value;
  };

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_basePort;
  int m_sampleRate;

// Platform-independent classes
protected:
  CAdLibCtlFSM m_AdLibFSM;

// Other member variables
protected:
  static bool isInstancesInitialized;
  static CAdLibCtl* instances[MAX_INSTANCES];
  static CCriticalSection OPLMutex;

  CThread m_playbackThread;
  TS_Queue<OPLMessage,OPL_QUEUE_LEN> m_OPLMsgQueue; // circular queue of OPL 'events'

  DWORD m_lastTime, m_curTime;
  double m_renderLoad;

  int m_instanceID;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IWAVELib::IWaveDataConsumerPtr m_waveOut;
};

#endif //__ADLIBCTL_H_
