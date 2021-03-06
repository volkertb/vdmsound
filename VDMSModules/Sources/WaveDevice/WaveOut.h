// WaveOut.h : Declaration of the CWaveOut

#ifndef __WAVEOUT_H_
#define __WAVEOUT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

/* TODO: put this in the .INI file ? */
#define WAVEOPEN_RETRY_INTERVAL   2

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IWave.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////
// CWaveOut
class ATL_NO_VTABLE CWaveOut : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWaveOut, &CLSID_WaveOut>,
  public IRunnable,
	public ISupportErrorInfo,
  public IVDMBasicModule,
  public IWaveDataConsumer
{
public:
	CWaveOut()
    : m_hWaveOut(NULL), m_deviceName(_T("<unknown>")), m_bufferedBytes(0)
  {
    m_waveFormat.nChannels = 0;
    m_waveFormat.nSamplesPerSec = 0;
    m_waveFormat.wBitsPerSample = 0;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_WAVEOUT)
DECLARE_NOT_AGGREGATABLE(CWaveOut)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWaveOut)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IWaveDataConsumer)
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

// IWaveDataConsumer
public:
  STDMETHOD(SetFormat)(WORD channels, DWORD samplesPerSec, WORD bitsPerSample);
  STDMETHOD(PlayData)(BYTE * data, LONG length, DOUBLE * load);

protected:
  static void CALLBACK WaveOutProc(HWAVEOUT hwo, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

protected:
  bool WaveOutOpen(bool isInteractive = true);
  void WaveOutClose(void);
  CString WaveOutGetName(void);
  CString WaveOutGetError(MMRESULT errCode);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_deviceID;
  int m_bufOpRange;

// Other member variables
protected:
  CString m_deviceName;
  HWAVEOUT m_hWaveOut;
  WAVEFORMATEX m_waveFormat;
  CThread m_gcThread;

  LONG m_bufferedBytes;
  LONG m_bufferedLo, m_bufferedHi;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IWaveDataConsumerPtr m_waveOut;
};

#endif //__WAVEOUT_H_
