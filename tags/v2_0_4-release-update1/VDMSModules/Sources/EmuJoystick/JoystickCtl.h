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

class CINIParser;

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
  // Information about the physical joystick devices (presence, capabilities,
  //  what axes and buttons are of interest, the axes and button values, etc.)
  struct JoyPhysicalInfo {
    bool isPresent;               // is the joystick available

    DWORD flags;                  // what information is of interest to us
    JOYCAPS caps;                 // Win32 device capabilities
    JOYINFOEX data;               // latest device data from Win32
  };

  // Types of analog data that can be obtained from a physical joystick device
  enum AnalogPortType {
    JOY_ANALOG_X, JOY_ANALOG_Y,
    JOY_ANALOG_Z, JOY_ANALOG_RUDDER,
    JOY_ANALOG_U, JOY_ANALOG_V,
    JOY_ANALOG_POV
  };

  // Types of digital data that can be obtained from a physical joystick device
  enum DigitalPortType {
    JOY_DIGITAL_BUTTON,
    JOY_DIGITAL_POV
  };

  // Information about one emulated analog input, which maps to
  //  one physical analog port
  struct AnalogInput {
    int value;                    // the continuous value of the port
    int mapJoyID;                 // the physical joystick this value should be read from
    AnalogPortType mapPortID;     // the source analogue port (axis) on the physical joystick
  };

  // Information about one emulated digital input, which maps to
  //  one physical digital port
  struct DigitalInput {
    bool value;                   // the discrete value of the port
    int mapJoyID;                 // the physical joystick this value should be read from
    DigitalPortType mapPortID;    // the source digital port (button) on the physical joystick
    int buttonID;                 // which button (or discrete POV bit) do we need
  };

  struct JoyEmuState {
    AnalogInput analog[4];        // emulated continuous value to be mapped on bits 0..3 (analogue) of the gameport
    DigitalInput digital[4];      // emulated discrete value to be mapped on bits 4..7 (digital) of the gameport

    JoyPhysicalInfo info[2];      // physical information about the josytick(s) as seen by the host OS
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
  HRESULT loadMapping(LPCSTR fName);
  HRESULT loadMappingEntry(LPCSTR fName, const CINIParser& map, bool isAnalog, int inputID);
  MMRESULT readJoyCaps(int joyID);
  MMRESULT readJoyData(int joyID, DWORD flagMask = JOY_RETURNALL);
  void resetJoystick(void);
  bool mapAnalogData(void);
  bool mapDigitalData(void);

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
  JoyEmuState m_state;        // joystick information
  bool m_pollRequest;

// Interfaces to dependency modules
protected:
  IVDMQUERYLib::IVDMRTEnvironmentPtr m_env;
  IVDMSERVICESLib::IVDMBaseServicesPtr m_BaseSrv;
  IVDMSERVICESLib::IVDMIOServicesPtr m_IOSrv;
};

#endif //__JOYSTICKCTL_H_
