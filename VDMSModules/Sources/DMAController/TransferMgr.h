// TransferMgr.h : Declaration of the CTransferMgr

#ifndef __TRANSFERMGR_H_
#define __TRANSFERMGR_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#define NUM_DMA_CHANNELS 8

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IDMAC.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
#import <IDMAHandlers.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////

struct DMAChannel {
  DMAChannel(void)
    : handler(NULL), isActive(false), inProgress(false), addr(0x0000), count(0xffff)
    { }
  IDMAHANDLERSLib::IDMAHandlerPtr handler;
  bool isActive;
  bool inProgress;
  WORD addr;
  WORD count;
};

/////////////////////////////////////////////////////////////////////////////
// CTransferMgr
class ATL_NO_VTABLE CTransferMgr : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTransferMgr, &CLSID_TransferMgr>,
  public IRunnable,
  public ISupportErrorInfo,
	public IVDMBasicModule,
	public IDMAController
{
public:
	CTransferMgr()
	{	}

DECLARE_REGISTRY_RESOURCEID(IDR_TRANSFERMGR)
DECLARE_NOT_AGGREGATABLE(CTransferMgr)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTransferMgr)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IVDMBasicModule)
	COM_INTERFACE_ENTRY(IDMAController)
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

// IDMAController
public:
	STDMETHOD(AddDMAHandler)(BYTE channel, IDMAHandler * handler);
	STDMETHOD(RemoveDMAHandler)(BYTE channel, IDMAHandler * handler);
	STDMETHOD(StartTransfer)(BYTE channel, LONG synchronous);
	STDMETHOD(StopTransfer)(BYTE channel, LONG synchronous);

protected:
  CThread m_DMAThread;
  CEvent m_event;
  CCriticalSection m_mutex;
  DWORD m_minPeriod, m_maxPeriod;
  double m_period, m_recoveryRate;
  DMAChannel m_channels[NUM_DMA_CHANNELS];

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMDMAServicesPtr m_DMASrv;
};

#endif //__TRANSFERMGR_H_
