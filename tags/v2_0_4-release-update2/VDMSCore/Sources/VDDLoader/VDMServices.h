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
  public IVDMIOServices,
  public IVDMDMAServices
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
  COM_INTERFACE_ENTRY(IVDMDMAServices)
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

// IVDMDMAServices
public:
  STDMETHOD(GetDMAState)(USHORT channel, DMA_INFO_T * DMAInfo);
  STDMETHOD(SetDMAState)(USHORT channel, DMA_INFO_SEL_T flags, DMA_INFO_T * DMAInfo);
  STDMETHOD(PerformDMATransfer)(USHORT channel, BYTE * buffer, ULONG length, ULONG * transferred);

/*
  STDMETHOD(AddMemHook)(ULONG baseAddr, ULONG addrRange, IMemHandler * handler);
  STDMETHOD(RemoveMemHook)(ULONG baseAddr, ULONG addrRange);
  STDMETHOD(AllocMem)(ULONG address, ULONG size);
  STDMETHOD(FreeMem)(ULONG address, ULONG size);
*/

#ifdef _VXD_SVC

protected:
  BOOL m_picUsage[2][8];  // what IRQ lines have been reserved
  BOOL m_dmaUsage[8];     // what DMA channels have been reserved

#endif //_VXD_SVC

protected:
// VDM utility types
  typedef DWORD (WINAPI* LPFNNTVDMCONTROL)(DWORD,LPVOID);

// VDM utility functions
  static DWORD VDMControl(DWORD fn, LPVOID param);

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

#ifdef _VXD_SVC

// VDD DMA hook functions
public:
  static VOID CALLBACK VDDDMAActivity(DWORD channel);

#endif //_VXD_SVC

protected:
  static CString StringFromPortRanges(int numPorts, const VDD_IO_PORTRANGE* ports);
  static USHORT getDOSEnvSeg(USHORT DOSPSPSeg);
  static CString getDOSEnvString(USHORT DOSPSPSeg, LPCSTR varName);
  static CString getDOSProgArg(USHORT DOSPSPSeg, int argIdx);

protected:
  HINSTANCE m_hInstance;

protected:
  static IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  static int m_fixPOPF;

protected:
  static long m_lInstanceCount;
  static bool m_isCommitted;
  static CIOPortMgr m_ports;

protected:
  static VDD_IO_HANDLERS m_hooks;
  static CArray<VDD_IO_PORTRANGE,VDD_IO_PORTRANGE&> m_ranges;
};

#endif //__VDMSERVICES_H_
