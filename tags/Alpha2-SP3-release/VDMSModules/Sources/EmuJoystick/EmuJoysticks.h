// EmuJoysticks.h : Deklaration von CEmuJoysticks

#ifndef __EMUJOYSTICKS_H_
#define __EMUJOYSTICKS_H_

#include "resource.h"       // Hauptsymbole



#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>
/////////////////////////////////////////////////////////////////////////////
// CEmuJoysticks
class ATL_NO_VTABLE CEmuJoysticks : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEmuJoysticks, &CLSID_EmuJoysticks>,
	public ISupportErrorInfo,
	public IVDMBasicModule,
	public IIOHandler
{
public:
	CEmuJoysticks()
	{
		m_J1XCountdown = 0;
		m_J1YCountdown = 0;
		m_J2XCountdown = 0;
		m_J2YCountdown = 0;
		m_J1Button1 = 0;
		m_J1Button2 = 0;
		m_J2Button1 = 0;
		m_J2Button2 = 0;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EMUJOYSTICKS)
DECLARE_NOT_AGGREGATABLE(CEmuJoysticks)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CEmuJoysticks)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
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
	IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
	IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
	IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;

	int m_NumJoysticks;
	int m_J1XCountdown, m_J1YCountdown, m_J2XCountdown, m_J2YCountdown;
	int m_J1Button1, m_J1Button2, m_J2Button1, m_J2Button2;
};

#endif //__EMUJOYSTICKS_H_
