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
    : m_lpDirectSound(NULL), m_lpDirectSoundBuffer(0), m_deviceGUID(GUID_NULL), m_deviceName(_T("<unknown>")), m_bufferLen(0), m_bufferPos(0), m_playedBytes(0), m_sentBytes(0), m_lastPlayPos(0)
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
  static BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext);

protected:
  bool DSoundOpen(bool isInteractive = true);
  void DSoundClose(void);
  HRESULT DSoundOpenHelper(void);
  CString DSoundGetName(LPGUID lpGUID = NULL);

/////////////////////////////////////////////////////////////////////////////

// Types
protected:
  struct DSDeviceInfo {
    DSDeviceInfo(const GUID& _guid = GUID_NULL, const CString& _description = _T(""), const CString& _module = _T(""))
      : guid(_guid), description(_description), module(_module)
      { }
    DSDeviceInfo(const DSDeviceInfo& src)
      : guid(src.guid), description(src.description), module(src.module)
      { }
    DSDeviceInfo& operator=(const DSDeviceInfo& src) {
      guid = src.guid;
      description = src.description;
      module = src.module;
      return *this;
    }

    GUID guid;
    CString description;
    CString module;
  };

  typedef CArray<DSDeviceInfo,DSDeviceInfo&> DSDeviceInfoArray;

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_deviceID;
  int m_bufOpRange;

// Other member variables
protected:
  CString m_deviceName;
  GUID m_deviceGUID;
  LPDIRECTSOUND m_lpDirectSound;
  LPDIRECTSOUNDBUFFER m_lpDirectSoundBuffer;
  WAVEFORMATEX m_waveFormat;

  CThread m_gcThread;
  CCriticalSection m_mutex;

  LONG m_bufferDuration;              // total length of audio buffer (in milliseconds)
  LONG m_bufferLen, m_bufferPos;      // size and current write position in buffer (in bytes)
  LONG m_DSoundLatency;               // maximum observed DSound latency (in bytes)
  LONG m_playedBytes, m_sentBytes;    // how many bytes of data were played (actually went through DSound device) to date and how many valid bytes were sent (produced)
  LONG m_lastPlayPos;                 // last known position of the play cursor (used to compute how many bytes went through the DSound device since the last check)
  LONG m_bufferedLo, m_bufferedHi;    // delimit the optimal range valid audio data should lead the play cursor by

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IWaveDataConsumerPtr m_waveOut;
};

#endif //__WAVEOUT_H_
