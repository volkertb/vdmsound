// MIDIDelay.h : Declaration of the CMIDIDelay

#ifndef __MIDIDELAY_H_
#define __MIDIDELAY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////
// CMIDIDelay
class ATL_NO_VTABLE CMIDIDelay : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CMIDIDelay, &CLSID_MIDIDelay>,
	public ISupportErrorInfo,
  public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
	CMIDIDelay()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIDELAY)
DECLARE_NOT_AGGREGATABLE(CMIDIDelay)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDIDelay)
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

/////////////////////////////////////////////////////////////////////////////

// Other member variables
protected:
  int m_delay;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MIDIDELAY_H_
