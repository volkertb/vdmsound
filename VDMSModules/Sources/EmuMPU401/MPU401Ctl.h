// MPU401Ctl.h : Declaration of the CMPU401Ctl

#ifndef __MPU401CTL_H_
#define __MPU401CTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "MPU401CtlFSM.h"

/////////////////////////////////////////////////////////////////////////////
// CMPU401Ctl
class ATL_NO_VTABLE CMPU401Ctl : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CMPU401Ctl, &CLSID_MPU401Ctl>,
  public IMPU401HWEmulationLayer,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler,
  public IMIDIEventHandler
{
public:
  CMPU401Ctl()
    : m_MPUFSM(this)
    { };

DECLARE_REGISTRY_RESOURCEID(IDR_MPU401CTL)
DECLARE_NOT_AGGREGATABLE(CMPU401Ctl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMPU401Ctl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
  COM_INTERFACE_ENTRY(IMIDIEventHandler)
END_COM_MAP()

// IMPU401HWEmulationLayer
public:
  void generateInterrupt(void);
  void logError(const char* message);
  void logWarning(const char* message);
  void putEvent(unsigned char status, unsigned char data1, unsigned char data2, unsigned char length);
  void putSysEx(const unsigned char * data, long length);
  void putRealTime(unsigned char data);

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

// IMIDIEventHandler
public:
  STDMETHOD(HandleEvent)(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length);
  STDMETHOD(HandleSysEx)(LONGLONG usDelta, BYTE * data, LONG length);
  STDMETHOD(HandleRealTime)(LONGLONG usDelta, BYTE data);

protected:
  CMPU401CtlFSM m_MPUFSM;
  int m_basePort;
  int m_IRQLine;

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MPU401CTL_H_
