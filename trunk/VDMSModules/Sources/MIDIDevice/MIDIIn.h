// MIDIIn.h : Declaration of the CMIDIIn

#ifndef __MIDIIN_H_
#define __MIDIIN_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

/* TODO: put this in the .INI file ? */
#define MIDIIN_OPEN_RETRY_INTERVAL   2

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////
// CMIDIIn
class ATL_NO_VTABLE CMIDIIn : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CMIDIIn, &CLSID_MIDIIn>,
  public IRunnable,
	public ISupportErrorInfo,
  public IVDMBasicModule
{
public:
	CMIDIIn()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIIN)
DECLARE_NOT_AGGREGATABLE(CMIDIIn)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDIIn)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
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

protected:
  static void CALLBACK MidiInProc(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

protected:
  bool MidiInOpen(bool isInteractive = true);
  void MidiInClose(void);
  CString MidiInGetName(void);
  CString MidiInGetError(MMRESULT errCode);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_deviceID;

// Other member variables
protected:
  CString m_deviceName;
  HMIDIIN m_hMidiIn;
  CThread m_recThread;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MIDIIN_H_
