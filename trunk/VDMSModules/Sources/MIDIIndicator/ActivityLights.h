// ActivityLights.h : Declaration of the CActivityLights

#ifndef __ACTIVITYLIGHTS_H_
#define __ACTIVITYLIGHTS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#define LED_NONE        -1
#define LED_NUMLOCK     VK_NUMLOCK
#define LED_CAPSLOCK    VK_CAPITAL
#define LED_SCROLLLOCK  VK_SCROLL

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////
// CActivityLights
class ATL_NO_VTABLE CActivityLights : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CActivityLights, &CLSID_ActivityLights>,
	public ISupportErrorInfo,
	public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
	CActivityLights()
    : ledID(LED_SCROLLLOCK), isIndicatorOn(false)
  {	};

DECLARE_REGISTRY_RESOURCEID(IDR_ACTIVITYLIGHTS)
DECLARE_NOT_AGGREGATABLE(CActivityLights)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CActivityLights)
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
  static bool __xor(bool a, bool b);
  static bool GetLedStatus(int ledID);
  static void SetLedStatus(int ledID, bool isLedOn);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_ledID;

// Other member variables
protected:
  bool m_isIndicatorOn;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__ACTIVITYLIGHTS_H_
