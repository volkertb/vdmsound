// JoystickCtl.cpp : Implementierung von CJoystickCtl
#include "stdafx.h"
#include "EmuJoystick.h"
#include "JoystickCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE       _T("The dependency module '%1' does not support the '%2' interface.%0")

#define MSG_ERR_IO_OPEN         _T("An error was encountered while attempting to open file '%1' for reading.%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")
#define MSG_ERR_IO              _T("Unknown I/O error encountered while accessing the file '%1':%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")
#define MSG_ERR_PARSE_SYNTAX    _T("Syntax error (%1):%nExpected '%3', but found '%2' instead.%0")
#define MSG_ERR_PARSE_FORMAT    _T("Unexpected data (%1):%nA non-empty line was encountered before the start of a section.%0")
#define MSG_ERR_PARSE_CONFLICT  _T("Symbol redefinition (%1):%nThe entry '%3' was already defined (see %2).%0")
#define MSG_ERR_PARSE           _T("Unknown parsing error (%1):%nDescription not available.%0")

#define MSG_ERR_MAP_SECTION     _T("Could not find the INI section [%2] in file '%1'.%0")
#define MSG_ERR_MAP_UNEXPECTED  _T("An unexpected condition was encountered while trying to access key '%3' in section [%2] of file '%1'.%0")
#define MSG_ERR_MAP_INVALIDFMT  _T("The value '%2' (see %1) is invalid.  Please provide a valid value.%0")
#define MSG_ERR_MAP_RANGE       _T("The value '%3' in '%2' (see %1) is invalid.  Please provide one of the following values: %4.%0")

#define MSG_ERR_MAP_RANGE_1     _T("'A' or 'B'")
#define MSG_ERR_MAP_RANGE_2     _T("'X', 'Y', 'Z', 'R' (rudder), 'U', 'V' or 'P' (POV)")
#define MSG_ERR_MAP_RANGE_3     _T("'B' (button) or 'P' (POV)")
#define MSG_ERR_MAP_RANGE_4     _T("'B1' (1st button), 'B2' (2nd button), ..., 'B31'")
#define MSG_ERR_MAP_RANGE_5     _T("'P1' (left/right) or 'P2' (front/rear)")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_SCALEMIN      L"minCoord"
#define INI_STR_SCALEMAX      L"maxCoord"
#define INI_STR_POLLINTERVAL  L"pollPeriod"
#define INI_STR_MAPFNAME      L"MapFile"

#define INI_STR_JOYMAP        "inputs"

/////////////////////////////////////////////////////////////////////////////

#define CHANGE_BIT(var, bit, set) ((set) ? ((var) | (1 << (bit))) : ((var) & (~(1 << (bit)))))

#define LIN_REMAP(oldVal, oldMin, oldMax, newMin, newMax) (((((oldVal) - (oldMin)) * ((newMax) - (newMin))) / ((oldMax) - (oldMin) + 1 /* prevents division by 0 */ )) + (newMin))

/////////////////////////////////////////////////////////////////////////////

#define JOY_RETURN_ANALOG  (JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNPOVCTS)
#define JOY_RETURN_DIGITAL (JOY_RETURNBUTTONS | JOY_RETURNPOV)

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

#include <INIParser.h>
#pragma comment ( lib , "INIParser.lib" )

/////////////////////////////////////////////////////////////////////////////
// CJoystickCtl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IIOHandler,
  };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IVDMBasicModule
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  HRESULT hr;

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends    = configuration; // Dependency query object
    Config     = configuration; // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain the joystick settings, use defaults if none specified
    m_basePort = CFG_Get(Config, INI_STR_BASEPORT, 0x201, 16, false);
    m_scaleMin = CFG_Get(Config, INI_STR_SCALEMIN, 15, 10, false);
    m_scaleMax = CFG_Get(Config, INI_STR_SCALEMAX, 575, 10, false);
    m_pollInterval = CFG_Get(Config, INI_STR_POLLINTERVAL, 125, 10, false);

    // Obtain the name of the file containing the mappings
    _bstr_t mapFileName = Config->Get(INI_STR_MAPFNAME);

    if (FAILED(hr = loadMapping(mapFileName)))
      return hr;

    /** Get VDM services ***************************************************/

    // Obtain VDM Services instance
    IUnknownPtr VDMServices
               = Depends->Get(INI_STR_VDMSERVICES);
    m_BaseSrv  = VDMServices;   // Base services (registers, interrupts, etc)
    m_IOSrv    = VDMServices;   // I/O services (I/O port hooks)

    if (m_BaseSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMBaseServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);
    if (m_IOSrv == NULL)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMIOServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  IVDMSERVICESLib::IIOHandler* pHandler = NULL; // IIOHandler interface to <this>

  // Register the I/O handlers
  try {
    // Obtain a COM IIOHandler interface on this C++ object
    if (FAILED(hr = QueryInterface(__uuidof(IVDMSERVICESLib::IIOHandler), (void**)(&pHandler))))
      throw _com_error(hr);     // Failure

    // Add this object as an I/O handler on the specified port range
    m_IOSrv->AddIOHook(m_basePort, 1, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

    pHandler->Release();        // Take back the AddRef in QueryInterface above
  } catch (_com_error& ce) {
    if (pHandler != NULL)
      pHandler->Release();      // Release the (unused) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
  }

  // Put the joystick(s) in a known state
  resetJoystick();

  // Create the joystick driver polling thread (regularly updates the locally-cached joystick state)
  m_joyPollThread.Create(this, true);   /* TODO: check that creation was successful */
  m_joyPollThread.SetPriority(THREAD_PRIORITY_NORMAL);
  m_joyPollThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("JoystickCtl initialized (joystick A %s, joystick B %s)"), m_state.info[0].isPresent ? (LPCTSTR)Format(_T("detected as '%s'"), m_state.info[0].caps.szPname) : _T("not detected"), m_state.info[1].isPresent ? (LPCTSTR)Format(_T("detected as '%s'"), m_state.info[1].caps.szPname) : _T("not detected")));

  return S_OK;
}

STDMETHODIMP CJoystickCtl::Destroy() {
  // Signal the polling thread to quit
  if (m_joyPollThread.GetThreadHandle() != NULL)
    m_joyPollThread.Cancel();

  // Release the VDM Services module
  m_IOSrv   = NULL;
  m_BaseSrv = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("JoystickCtl  released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IIOHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::HandleINB(USHORT inPort, BYTE * data) {
  if (data == NULL)
    return E_POINTER;

  switch (inPort - m_basePort) {
    case 0:   // return button state / axis bits

      /**
       **  Format of the byte to be returned:
       **
       **                        +-------------------------------+
       **                        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
       **                        +-------------------------------+
       **                          |   |   |   |   |   |   |   |
       **  Joystick B, Button 2 ---+   |   |   |   |   |   |   +--- Joystick A, X Axis
       **  Joystick B, Button 1 -------+   |   |   |   |   +------- Joystick A, Y Axis
       **  Joystick A, Button 2 -----------+   |   |   +----------- Joystick B, X Axis
       **  Joystick A, Button 1 ---------------+   +--------------- Joystick B, Y Axis
       **/

      *data = 0xff;

      m_pollRequest = true;   // request an asynchronous update on the joystick buttons' state

      *data = (BYTE)CHANGE_BIT(*data, 0, m_state.analog[0].value > 0);
      *data = (BYTE)CHANGE_BIT(*data, 1, m_state.analog[1].value > 0);
      *data = (BYTE)CHANGE_BIT(*data, 2, m_state.analog[2].value > 0);
      *data = (BYTE)CHANGE_BIT(*data, 3, m_state.analog[3].value > 0);

      *data = (BYTE)CHANGE_BIT(*data, 4, !m_state.digital[0].value);
      *data = (BYTE)CHANGE_BIT(*data, 5, !m_state.digital[1].value);
      *data = (BYTE)CHANGE_BIT(*data, 6, !m_state.digital[2].value);
      *data = (BYTE)CHANGE_BIT(*data, 7, !m_state.digital[3].value);

      if (m_state.analog[0].value > 0) m_state.analog[0].value--;
      if (m_state.analog[1].value > 0) m_state.analog[1].value--;
      if (m_state.analog[2].value > 0) m_state.analog[2].value--;
      if (m_state.analog[3].value > 0) m_state.analog[3].value--;

      return S_OK;

    default:
      *data = 0xff;
      return E_FAIL;
  }
}

STDMETHODIMP CJoystickCtl::HandleOUTB(USHORT outPort, BYTE data) {
  switch (outPort - m_basePort) {
    case 0:
      m_mutex.Lock();           // gain exclusive access to joystick state
      readJoyData(0, JOY_RETURNALL);  // read information from Win32 about
      readJoyData(1, JOY_RETURNALL);  //  both buttons and coordinates.
      mapAnalogData();          // update emulated information about
      mapDigitalData();         //  both buttons and coordinates.
      m_mutex.Unlock();         // release joystick state after having updated it

      return S_OK;

    default:
      return E_FAIL;
  }
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::HandleINW(USHORT inPort, USHORT * data) {
  if (data == NULL) return E_POINTER;
  *data = -1;
  return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  memset(data, -1, count * sizeof(data[0]));
  return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTW(USHORT outPort, USHORT data) {
  return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
  if (data == NULL) return E_POINTER;
  return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CJoystickCtl::Run(CThread& thread) {
  MSG message;

  _ASSERTE(thread.GetThreadID() == m_joyPollThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Polling thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  do {
    if (thread.GetMessage(&message, false)) {       // non-blocking (message 'peek')
      switch (message.message) {
        case WM_QUIT:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Polling thread cancelled")));
          return 0;

        default:
          break;
      }
    } else {

      // Optimization: we could update the joystick buttons' state every m_pollInterval
      //  milliseconds; if, however, nobody is reading from the joystick port, we would
      //  be calling the joystick driver gratuitously.  We therefore check a simple boolean
      //  flag that is set whenever the joystick port is read, which tells us that a state
      //  update is desirable (we do not use more sophisticated mechanisms for performance
      //  reasons, as the joystick port is usually read at microsecond (!) intervals).  As
      //  soon as the update is performed, we reset the boolean flag.

      if (m_pollRequest) {      // check if an asynchronous state update is requested
        m_mutex.Lock();         // gain exclusive access to joystick state
        readJoyData(0, JOY_RETURN_DIGITAL);   // read button information,
        readJoyData(1, JOY_RETURN_DIGITAL);   //  but not the coordinate data.
        mapDigitalData();       // only update button data.
        m_mutex.Unlock();       // release joystick state after having updated it

        m_pollRequest = false;  // the update is complete => reset the request flag
      }

      Sleep((DWORD)m_pollInterval);
    }
  } while (true);
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

HRESULT CJoystickCtl::loadMapping(LPCSTR fName) {
  int inputID;

  m_state.info[0].flags = 0;
  m_state.info[1].flags = 0;

  // Open the .INI file containig the mapping information
  CINIParser map;

  try {
    map.load(SearchPathA(fName));
  } catch (CINIParser::fopen_error& foe) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_IO_OPEN, /*false, NULL, 0, */false, (LPCTSTR)CString(foe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::io_error& ioe) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_IO, /*false, NULL, 0, */false, (LPCTSTR)CString(ioe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::syntax_error& se) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_SYNTAX, /*false, NULL, 0, */false, (LPCTSTR)CString(se.location.c_str()), (LPCTSTR)CString(se.offender.c_str()), (LPCTSTR)CString(se.expected.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::format_error& fe) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_FORMAT, /*false, NULL, 0, */false, (LPCTSTR)CString(fe.location.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::conflict_error& ce) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_CONFLICT, /*false, NULL, 0, */false, (LPCTSTR)CString(ce.location.c_str()), (LPCTSTR)CString(ce.defLocation.c_str()), (LPCTSTR)CString(ce.redefined.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::parse_error& pe) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE, /*false, NULL, 0, */false, (LPCTSTR)CString(pe.location.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  }

  HRESULT hr;

  for (inputID = 0; inputID < 4; inputID++) {
    if (FAILED(hr = loadMappingEntry(fName, map, true, inputID)))
      return hr;
    if (FAILED(hr = loadMappingEntry(fName, map, false, inputID)))
      return hr;
  }

  return S_OK;
}

//
//
//
HRESULT CJoystickCtl::loadMappingEntry(LPCSTR fName, const CINIParser& map, bool isAnalog, int inputID) {
  char key[256];
  std::string value, lowcaseValue, keyLoc;
  std::string::iterator itValue;

  if (isAnalog) {
    sprintf(key, "analog.%d", inputID + 1);
  } else {
    sprintf(key, "digital.%d", inputID + 1);
  }

  if (!map.getLocation(INI_STR_JOYMAP, key, keyLoc))
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_UNEXPECTED, /*false, NULL, 0, */false, (LPCTSTR)CString(fName), (LPCTSTR)CString(INI_STR_JOYMAP), (LPCTSTR)CString(key)), __uuidof(IVDMBasicModule), E_UNEXPECTED);
  if (!map.getValue(INI_STR_JOYMAP, key, value))
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_UNEXPECTED, /*false, NULL, 0, */false, (LPCTSTR)CString(fName), (LPCTSTR)CString(INI_STR_JOYMAP), (LPCTSTR)CString(key)), __uuidof(IVDMBasicModule), E_UNEXPECTED);

  for (itValue = value.begin(); itValue != value.end(); itValue++) {
    lowcaseValue += tolower(*itValue);  // make a lowercase copy of value in lowcaseValue
  }

  if (isAnalog) {
    int mapJoyID;
    char mapJoyID_chr, mapPortID_chr;
    AnalogPortType mapPortID;
    DWORD flag;

    if (sscanf(lowcaseValue.c_str(), "%c.analog.%c", &mapJoyID_chr, &mapPortID_chr) != 2)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_INVALIDFMT, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str())), __uuidof(IVDMBasicModule), E_ABORT);

    switch (mapJoyID_chr) {
      case 'a': mapJoyID = 0; break;
      case 'b': mapJoyID = 1; break;
      default : return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Left(2)), MSG_ERR_MAP_RANGE_1), __uuidof(IVDMBasicModule), E_ABORT);
    }

    switch (mapPortID_chr) {
      case 'x': mapPortID = JOY_ANALOG_X; flag = JOY_RETURNX; break;
      case 'y': mapPortID = JOY_ANALOG_Y; flag = JOY_RETURNY; break;
      case 'z': mapPortID = JOY_ANALOG_Z; flag = JOY_RETURNZ; break;
      case 'r': mapPortID = JOY_ANALOG_RUDDER; flag = JOY_RETURNR; break;
      case 'u': mapPortID = JOY_ANALOG_U; flag = JOY_RETURNU; break;
      case 'v': mapPortID = JOY_ANALOG_V; flag = JOY_RETURNV; break;
      case 'p': mapPortID = JOY_ANALOG_POV; flag = JOY_RETURNPOV | JOY_RETURNPOVCTS; break;
      default : return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Right(2)), MSG_ERR_MAP_RANGE_2), __uuidof(IVDMBasicModule), E_ABORT);
    }

    m_state.info[mapJoyID].flags |= flag;

    m_state.analog[inputID].value = 0;
    m_state.analog[inputID].mapJoyID = mapJoyID;
    m_state.analog[inputID].mapPortID = mapPortID;

    return S_OK;
  } else {
    int mapJoyID, buttonID;
    char mapJoyID_chr, mapPortID_chr;
    DigitalPortType mapPortID;
    DWORD flag;

    if (sscanf(lowcaseValue.c_str(), "%c.digital.%c%d", &mapJoyID_chr, &mapPortID_chr, &buttonID) != 3)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_INVALIDFMT, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str())), __uuidof(IVDMBasicModule), E_ABORT);

    switch (mapJoyID_chr) {
      case 'a': mapJoyID = 0; break;
      case 'b': mapJoyID = 1; break;
      default : return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Left(2)), MSG_ERR_MAP_RANGE_1), __uuidof(IVDMBasicModule), E_ABORT);
    }

    switch (mapPortID_chr) {
      case 'b': mapPortID = JOY_DIGITAL_BUTTON; flag = JOY_RETURNBUTTONS; break;
      case 'p': mapPortID = JOY_DIGITAL_POV; flag = JOY_RETURNPOV | JOY_RETURNPOVCTS; break;
      default : return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Right(3)), MSG_ERR_MAP_RANGE_3), __uuidof(IVDMBasicModule), E_ABORT);
    }

    if ((mapPortID_chr == 'b') && ((buttonID < 0) || (buttonID > 31)))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Right(3)), MSG_ERR_MAP_RANGE_4), __uuidof(IVDMBasicModule), E_ABORT);

    if ((mapPortID_chr == 'p') && ((buttonID < 0) || (buttonID > 1)))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str()), (LPCTSTR)(CString(value.c_str()).Right(3)), MSG_ERR_MAP_RANGE_5), __uuidof(IVDMBasicModule), E_ABORT);

    m_state.info[mapJoyID].flags |= flag;

    m_state.digital[inputID].value = false;
    m_state.digital[inputID].mapJoyID = mapJoyID;
    m_state.digital[inputID].mapPortID = mapPortID;
    m_state.digital[inputID].buttonID = buttonID - 1;   // convert from 1-based to 0-based indices

    return S_OK;
  }
}

//
// Retrieves calibration data from the Windows driver
//
MMRESULT CJoystickCtl::readJoyCaps(
  int joyID )           // Identifier of the joystick (JOYSTICKID1 or JOYSTICKID2) to be queried
{
  MMRESULT errCode;
  UINT joyID_win32[2] = { JOYSTICKID1, JOYSTICKID2 };
  JOYCAPS* pJoyInfo = &(m_state.info[joyID].caps);

  if ((errCode = joyGetDevCaps(joyID_win32[joyID], pJoyInfo, sizeof(*pJoyInfo))) != JOYERR_NOERROR) {
    m_state.info[joyID].isPresent = false;
  } else {
    m_state.info[joyID].isPresent = true;
  }

  return errCode;
}

//
//
//
MMRESULT CJoystickCtl::readJoyData(
  int joyID,            // Identifier of the joystick (JOYSTICKID1 or JOYSTICKID2) to be queried
  DWORD flagMask )      // Indicates whether to update *emulated* analog (coordinate) and/or digital (button) information
{
  MMRESULT errCode;
  UINT joyID_win32[2] = { JOYSTICKID1, JOYSTICKID2 };
  JOYINFOEX* pJoyInfo = &(m_state.info[joyID].data);

  memset(pJoyInfo, 0, sizeof(*pJoyInfo));
  pJoyInfo->dwSize = sizeof(*pJoyInfo);
  pJoyInfo->dwFlags = m_state.info[joyID].flags & flagMask;

  if ((errCode = joyGetPosEx(joyID_win32[joyID], pJoyInfo)) != JOYERR_NOERROR) {
    m_state.info[joyID].isPresent = false;
  } else {
    m_state.info[joyID].isPresent = true;
  }

  return errCode;
}

//
// Reset the joystick state (all capacitors dicharged, all buttons released),
//   and reload calibration data (min/max X-Y information, etc.)
//
void CJoystickCtl::resetJoystick(void) {
  // Refresh joystick calibration data
  readJoyCaps(0);
  readJoyCaps(1);

  // Pretend the capacitors have discharged by now, i.e. the counters expired
  m_state.analog[0].value = 0;
  m_state.analog[1].value = 0;
  m_state.analog[2].value = 0;
  m_state.analog[3].value = 0;

  // Pretend the buttons are not pressed
  m_state.digital[0].value = false;
  m_state.digital[1].value = false;
  m_state.digital[2].value = false;
  m_state.digital[3].value = false;
}

//
//
//
bool CJoystickCtl::mapAnalogData(void) {
  for (int inputID = 0; inputID < 4; inputID++) {
    AnalogInput& input = m_state.analog[inputID];
    bool isJoyPresent = m_state.info[input.mapJoyID].isPresent;
    JOYINFOEX& joyInfo = m_state.info[input.mapJoyID].data;
    JOYCAPS& joyCaps = m_state.info[input.mapJoyID].caps;

    switch (input.mapPortID) {
      case JOY_ANALOG_X:
        if (isJoyPresent) {
          input.value = LIN_REMAP((int)joyInfo.dwXpos, joyCaps.wXmin, joyCaps.wXmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;  // emulate maximum input resistance
        } break;
      case JOY_ANALOG_Y:
        if (isJoyPresent) {
          input.value = LIN_REMAP((int)joyInfo.dwYpos, joyCaps.wYmin, joyCaps.wYmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      case JOY_ANALOG_Z:
        if (isJoyPresent && ((joyCaps.wCaps & JOYCAPS_HASZ) != 0)) {
          input.value = LIN_REMAP((int)joyInfo.dwZpos, joyCaps.wZmin, joyCaps.wZmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      case JOY_ANALOG_RUDDER:
        if (isJoyPresent && ((joyCaps.wCaps & JOYCAPS_HASR) != 0)) {
          input.value = LIN_REMAP((int)joyInfo.dwRpos, joyCaps.wRmin, joyCaps.wRmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      case JOY_ANALOG_U:
        if (isJoyPresent && ((joyCaps.wCaps & JOYCAPS_HASU) != 0)) {
          input.value = LIN_REMAP((int)joyInfo.dwUpos, joyCaps.wUmin, joyCaps.wUmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      case JOY_ANALOG_V:
        if (isJoyPresent && ((joyCaps.wCaps & JOYCAPS_HASV) != 0)) {
          input.value = LIN_REMAP((int)joyInfo.dwVpos, joyCaps.wVmin, joyCaps.wVmax, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      case JOY_ANALOG_POV:
        if (isJoyPresent && ((joyCaps.wCaps & JOYCAPS_HASPOV) != 0)) {
          input.value = LIN_REMAP((int)joyInfo.dwPOV, 0, 35900, m_scaleMin, m_scaleMax);
        } else {
          input.value = INT_MAX;
        } break;
      default:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("mapAnalogData: invalid port ID %d (0x%08x), state corrupted"), input.mapPortID, input.mapPortID));
        input.value = 0;
        return false;
    }
  }

  return true;
}

//
//
//
bool CJoystickCtl::mapDigitalData(void) {
  for (int inputID = 0; inputID < 4; inputID++) {
    DigitalInput& input = m_state.digital[inputID];
    bool isJoyPresent = m_state.info[input.mapJoyID].isPresent;
    JOYINFOEX& joyInfo = m_state.info[input.mapJoyID].data;

    switch (input.mapPortID) {
      case JOY_DIGITAL_BUTTON:
        if (isJoyPresent) {
          input.value = (joyInfo.dwButtons & (1 << input.buttonID)) != 0;
        } else {
          input.value = false;  // emulate open circuit
        } break;
      case JOY_DIGITAL_POV:
        if (isJoyPresent) {
          if ((joyInfo.dwPOV < 45*100) || (joyInfo.dwPOV >= 315*00)) {
            input.value = (0 & (1 << input.buttonID)) != 0;
          } else if (joyInfo.dwPOV < 135*100) {
            input.value = (1 & (1 << input.buttonID)) != 0;
          } else if (joyInfo.dwPOV < 225*100) {
            input.value = (2 & (1 << input.buttonID)) != 0;
          } else if (joyInfo.dwPOV < 315*100) {
            input.value = (3 & (1 << input.buttonID)) != 0;
          }
        } else {
          input.value = false;
        } break;
      default:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("mapDigitalData: invalid port ID %d (0x%08x), state corrupted"), input.mapPortID, input.mapPortID));
        input.value = false;
        return false;
    }
  }

  return true;
}
