// MIDIMapper.h : Declaration of the CMIDIMapper

#ifndef __MIDIMAPPER_H_
#define __MIDIMAPPER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////
// CMIDIMapper
class ATL_NO_VTABLE CMIDIMapper : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CMIDIMapper, &CLSID_MIDIMapper>,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
  CMIDIMapper()
  {
  }

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIMAPPER)
DECLARE_NOT_AGGREGATABLE(CMIDIMapper)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDIMapper)
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
  HRESULT loadMapping(LPCSTR fName);

protected:
  /* TODO: also implement volume map, drum map, etc. */
  BYTE m_patchMap[128];

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MIDIMAPPER_H_
