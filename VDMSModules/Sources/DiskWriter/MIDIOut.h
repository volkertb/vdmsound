// MIDIOut.h : Declaration of the CMIDIOut

#ifndef __MIDIOUT_H_
#define __MIDIOUT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IMIDI.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include "DiskUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CMIDIOut
class ATL_NO_VTABLE CMIDIOut : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CMIDIOut, &CLSID_MIDIOut>,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IMIDIEventHandler
{
public:
	CMIDIOut() : m_dataLen(0)
	{	}

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIOUT)
DECLARE_NOT_AGGREGATABLE(CMIDIOut)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMIDIOut)
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
  void NewFile(void);
  void WriteSysEx(LONGLONG usDelta, const BYTE* data, LONG length);
  void WriteMeta(LONGLONG usDelta, BYTE metaCmd, const BYTE* data, LONG length);
  void WriteEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length);
  void SaveFile(void);

protected:
  void InitHeader(void);
  void IncreaseTrackLength(LONG delta);
  int GetVarLenVal(const BYTE* varSrc, int nCount, BYTE* varDest);
  static int GetNumSignificantBits(const BYTE* varSrc, int nCount);
  static BYTE GetUnalignedByte(const BYTE* varSrc, int bitPos);

/////////////////////////////////////////////////////////////////////////////

// Other member variables
protected:
  CString m_path;
  CSequentialFile m_fOut;

  LONGLONG m_lastTime;

# pragma pack (push, 1)

  struct {
    // File header
    DWORD dwStructSignature;
    DWORD dwStructLength;
    WORD  wFileFormat;
    WORD  wTrackCount;
    WORD  wDeltaTimeTicks;

    // Track header
    DWORD dwTrackSignature;
    DWORD dwTrackLength;
  } m_midHdr;

# pragma pack (pop)

  BYTE m_dataBuf[65536];
  LONG m_dataLen;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IMIDIEventHandlerPtr m_midiOut;
};

#endif //__MIDIOUT_H_
