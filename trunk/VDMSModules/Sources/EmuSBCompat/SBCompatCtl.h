// SBCompatCtl.h : Declaration of the CSBCompatCtl

#ifndef __SBCOMPATCTL_H_
#define __SBCOMPATCTL_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "SBCompatCtlDSP.h"
#include "SBCompatCtlMixer.h"

/////////////////////////////////////////////////////////////////////////////
// CSBCompatCtl
class ATL_NO_VTABLE CSBCompatCtl : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CSBCompatCtl, &CLSID_SBCompatCtl>,
  public ISBDSPHWEmulationLayer,
  public ISBMixerHWEmulationLayer,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler
{
public:
  CSBCompatCtl()
    : m_SBDSP(this), m_SBMixer(this)
    { };

DECLARE_REGISTRY_RESOURCEID(IDR_SBCOMPATCTL)
DECLARE_NOT_AGGREGATABLE(CSBCompatCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSBCompatCtl)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
END_COM_MAP()

// ISBDSPHWEmulationLayer, ISBMixerHWEmulationLayer
public:
  void logError(const char* message);
  void logWarning(const char* message);

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
  CSBCompatCtlDSP m_SBDSP;
  CSBCompatCtlMixer m_SBMixer;
  int m_basePort;
  int m_IRQLine;
  int m_DMA8Channel;
  int m_DMA16Channel;

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
};

#endif //__SBCOMPATCTL_H_
