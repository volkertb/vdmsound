// JoystickCtl.h : Deklaration von CJoystickCtl

#ifndef __JOYSTICKCTL_H_
#define __JOYSTICKCTL_H_

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4192 )
#import <IVDMModule.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import <IVDMHandlers.tlb> raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#import <IVDMServices.tlb>
#import <IVDMQuery.tlb>

/////////////////////////////////////////////////////////////////////////////

#include <Thread.h>

/////////////////////////////////////////////////////////////////////////////
// CJoystickCtl
class ATL_NO_VTABLE CJoystickCtl : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CJoystickCtl, &CLSID_JoystickCtl>,
  public IRunnable,
  public ISupportErrorInfo,
  public IVDMBasicModule,
  public IIOHandler
{
protected:
  struct JoyInfo {
    bool isPresent;     // is the joystick available ?
    int XCount, YCount; // (X,Y) position counters
    bool but1, but2;    // joystick buttons state
    JOYCAPS caps;       // Windows device capabilities
    JOYINFO state;      // latest device data from Windows
  };

public:
  CJoystickCtl()
    : m_pollRequest(true)
    { }

DECLARE_REGISTRY_RESOURCEID(IDR_JOYSTICKCTL)
DECLARE_NOT_AGGREGATABLE(CJoystickCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CJoystickCtl)
  COM_INTERFACE_ENTRY(IVDMBasicModule)
  COM_INTERFACE_ENTRY(IIOHandler)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
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
  void resetJoystick(void);
  MMRESULT updateJoyCaps(UINT joyID, JoyInfo& joyInfo);
  MMRESULT updateJoyState(UINT joyID, JoyInfo& joyInfo, bool doCoords = true, bool doButtons = true);

/////////////////////////////////////////////////////////////////////////////

// Module's settings
protected:
  int m_basePort;             // base port where the joystick can be accessed by DOS programs
  int m_scaleMin, m_scaleMax; // range of values for the X and Y intervals reported to DOS programs
  int m_pollInterval;         // how many milliseconds to sleep between joystick polls

// Other member variables
protected:
  CThread m_joyPollThread;    // thread that regularly polls the Windows driver
  CCriticalSection m_mutex;   // regulates concurrent access to the device data in m_joyInfo (regulate access by the poll thread + forced access by some I/O traps)
  JoyInfo m_joyInfo[2];       // joystick information
  bool m_pollRequest;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
};

#endif //__JOYSTICKCTL_H_
