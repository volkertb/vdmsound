// WaveOut.h : Declaration of the CWaveOut

#ifndef __WAVEOUT_H_
#define __WAVEOUT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IWave.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "DiskUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveOut
class ATL_NO_VTABLE CWaveOut : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWaveOut, &CLSID_WaveOut>,
	public ISupportErrorInfo,
  public IVDMBasicModule,
  public IWaveDataConsumer
{
public:
  CWaveOut() : m_dataLen(0)
	{	}

DECLARE_REGISTRY_RESOURCEID(IDR_WAVEOUT)
DECLARE_NOT_AGGREGATABLE(CWaveOut)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWaveOut)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IWaveDataConsumer)
END_COM_MAP()

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
  void NewFile(WORD channels, DWORD samplesPerSec, WORD bitsPerSample);
  void AddCue(void);
  void WriteData(BYTE * data, LONG length);
  void SaveFile(void);

protected:
  void InitHeader(WORD channels, DWORD samplesPerSec, WORD bitsPerSample);

/////////////////////////////////////////////////////////////////////////////

// Other member variables
protected:
  CString m_path;
  CSequentialFile m_fOut;

  CDWordArray m_cues;

# pragma pack (push, 1)

  struct {
    // RIFF chunk
    DWORD dwRiffChunkID;
    DWORD dwRiffChunkSize;

    DWORD dwRiffFormat;

    // FORMAT chunk
    DWORD dwFmtChunkID;
    DWORD dwFmtChunkSize;

    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;

    // DATA chunk
    DWORD dwDataChunkID;
    DWORD dwDataChunkSize;
  } m_wavHdr;

  struct {
    // CUE chunk
    DWORD dwCueChunkID;
    DWORD dwCueChunkSize;

    DWORD dwCuePoints;
  } m_wavTail;

# pragma pack (pop)

  BYTE m_dataBuf[65536];
  LONG m_dataLen;
  CCriticalSection m_lock;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IWaveDataConsumerPtr m_waveOut;
};

#endif //__WAVEOUT_H_
