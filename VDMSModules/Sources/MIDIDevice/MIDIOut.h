// MIDIOut.h : Declaration of the CMIDIOut

#ifndef __MIDIOUT_H_
#define __MIDIOUT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

/* TODO: put this in the .INI file ? */
#define MIDIOPEN_RETRY_INTERVAL   2

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////
// CMIDIOut
class ATL_NO_VTABLE CMIDIOut : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CMIDIOut, &CLSID_MIDIOut>,
  public IRunnable,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
  CMIDIOut()
    : m_hMidiOut(NULL), m_deviceName(_T("<unknown>"))
    { };

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIOUT)
DECLARE_NOT_AGGREGATABLE(CMIDIOut)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDIOut)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IMIDIEventHandler)
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

// IMIDIEventHandler
public:
  STDMETHOD(HandleEvent)(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length);
  STDMETHOD(HandleSysEx)(LONGLONG usDelta, BYTE * data, LONG length);
  STDMETHOD(HandleRealTime)(LONGLONG usDelta, BYTE data);

protected:
  void MidiOutOpen(void);
  void MidiOutClose(void);
  CString MidiOutGetName(void);
  CString MidiOutGetError(MMRESULT errCode);

protected:
  int m_deviceID;
  CString m_deviceName;
  HMIDIOUT m_hMidiOut;
  CThread gcThread;

protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MIDIOUT_H_
