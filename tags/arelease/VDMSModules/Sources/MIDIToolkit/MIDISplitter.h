// MIDISplitter.h : Declaration of the CMIDISplitter

#ifndef __MIDISPLITTER_H_
#define __MIDISPLITTER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////
// CMIDISplitter
class ATL_NO_VTABLE CMIDISplitter : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CMIDISplitter, &CLSID_MIDISplitter>,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
  CMIDISplitter()
  {
  }

DECLARE_REGISTRY_RESOURCEID(IDR_MIDISPLITTER)
DECLARE_NOT_AGGREGATABLE(CMIDISplitter)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDISplitter)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IMIDIEventHandler)
END_COM_MAP()

// ISupportsErrorInfo
public:
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IVDMBasicModule
public:
  STDMETHOD(Init)(IUnknown * configuration);
  STDMETHOD(Destroy)();

// IMIDIEventHandler
public:
  STDMETHOD(HandleEvent)(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length);
  STDMETHOD(HandleSysEx)(LONGLONG usDelta, BYTE * data, LONG length);
  STDMETHOD(HandleRealTime)(LONGLONG usDelta, BYTE data);

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  /* TODO: add array of IMIDIEventHandlerPtr's */
};

#endif //__MIDISPLITTER_H_
