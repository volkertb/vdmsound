// VDMServices.h : Declaration of the CVDMServices

#ifndef __VDMSERVICES_H_
#define __VDMSERVICES_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMServices.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "IOPortMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CVDMServices
class ATL_NO_VTABLE CVDMServices : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CVDMServices, &CLSID_VDMServices>,
	public ISupportErrorInfo,
	public IVDMBasicModule,
	public IVDMBaseServices,
  public IVDMIOServices
{
public:
  CVDMServices() : m_hInstance(NULL)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VDMSERVICES)
DECLARE_NOT_AGGREGATABLE(CVDMServices)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CVDMServices)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IVDMBasicModule)
	COM_INTERFACE_ENTRY(IVDMBaseServices)
	COM_INTERFACE_ENTRY(IVDMIOServices)
END_COM_MAP()

// ISupportsErrorInfo
public:
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IVDMBasicModule
public:
	STDMETHOD(Init)(IUnknown * configuration);
	STDMETHOD(Destroy)();

// IVDMBaseServices
public:
	STDMETHOD(GetRegister)(REGISTER_T reg, ULONG * value);
	STDMETHOD(SetRegister)(REGISTER_T reg, ULONG value);
	STDMETHOD(GetFlag)(FLAG_T flag, ULONG * value);
	STDMETHOD(SetFlag)(FLAG_T flag, ULONG value);
	STDMETHOD(GetMemory)(WORD segment, ULONG offset, ADDRMODE_T mode, BYTE * buffer, ULONG length);
	STDMETHOD(SetMemory)(WORD segment, ULONG offset, ADDRMODE_T mode, BYTE * buffer, ULONG length);
	STDMETHOD(SimulateInterrupt)(INTERRUPT_T type, BYTE line, USHORT count);
	STDMETHOD(TerminateVDM)();

// IVDMIOServices
public:
	STDMETHOD(AddIOHook)(WORD basePort, WORD portRange, OPERATIONS_T inOps, OPERATIONS_T outOps, IIOHandler * handler);
	STDMETHOD(RemoveIOHook)(WORD basePort, WORD portRange, IIOHandler * handler);

/*
	STDMETHOD(AddIOHook)(USHORT basePort, USHORT portRange, IIOHandler * handler);
	STDMETHOD(RemoveIOHook)(USHORT basePort, USHORT portRange);
	STDMETHOD(AddMemHook)(ULONG baseAddr, ULONG addrRange, IMemHandler * handler);
	STDMETHOD(RemoveMemHook)(ULONG baseAddr, ULONG addrRange);
	STDMETHOD(AllocMem)(ULONG address, ULONG size);
	STDMETHOD(FreeMem)(ULONG address, ULONG size);
	STDMETHOD(QueryDMA)(USHORT channel, DMA_INFO_T * DMAInfo);
	STDMETHOD(SetDMA)(USHORT channel, DMA_INFO_SEL_T flags, DMA_INFO_T * DMAInfo);
	STDMETHOD(RequestDMA)(USHORT channel, BYTE * buffer, ULONG length);
*/

// VDD user hook functions
public:
  static VOID CALLBACK VDDUserCreate(USHORT DosPDB);
  static VOID CALLBACK VDDUserTerminate(USHORT DosPDB);
  static VOID CALLBACK VDDUserBlock(VOID);
  static VOID CALLBACK VDDUserResume(VOID);

// VDD IO hook functions
public:
  static VOID CALLBACK VDDPortINB(WORD iPort, BYTE * data);
  static VOID CALLBACK VDDPortINW(WORD iPort, WORD * data);
  static VOID CALLBACK VDDPortINSB(WORD iPort, BYTE * data, WORD count);
  static VOID CALLBACK VDDPortINSW(WORD iPort, WORD * data, WORD count);

  static VOID CALLBACK VDDPortOUTB(WORD oPort, BYTE data);
  static VOID CALLBACK VDDPortOUTW(WORD oPort, WORD data);
  static VOID CALLBACK VDDPortOUTSB(WORD oPort, BYTE * data, WORD count);
  static VOID CALLBACK VDDPortOUTSW(WORD oPort, WORD * data, WORD count);

protected:
  static CString StringFromPortRanges(int numPorts, const VDD_IO_PORTRANGE* ports);

protected:
	HINSTANCE m_hInstance;

protected:
  static IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;

protected:
  static long m_lInstanceCount;
  static bool m_isCommitted;
  static CIOPortMgr m_ports;

protected:
  static VDD_IO_HANDLERS m_hooks;
  static CArray<VDD_IO_PORTRANGE,VDD_IO_PORTRANGE&> m_ranges;
};

#endif //__VDMSERVICES_H_
