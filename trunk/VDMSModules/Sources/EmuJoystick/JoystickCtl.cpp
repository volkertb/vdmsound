// JoystickCtl.cpp : Implementierung von CJoystickCtl
#include "stdafx.h"
#include "EmuJoystick.h"
#include "JoystickCtl.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"

#define INI_STR_BASEPORT      L"port"
#define INI_STR_SCALEMIN      L"minCoord"
#define INI_STR_SCALEMAX      L"maxCoord"
#define INI_STR_POLLINTERVAL  L"pollPeriod"

/////////////////////////////////////////////////////////////////////////////

#define CHANGE_BIT(var, bit, set) ((set) ? ((var) | (1 << (bit))) : ((var) & (~(1 << (bit)))))

#define LIN_REMAP(oldVal, oldMin, oldMax, newMin, newMax) (((((oldVal) - (oldMin)) * ((newMax) - (newMin))) / ((oldMax) - (oldMin))) + (newMin))

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

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

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("JoystickCtl initialized (joystick A %s, joystick B %s)"), m_joyInfo[0].isPresent ? (LPCTSTR)Format(_T("detected as '%s'"), m_joyInfo[0].caps.szPname) : _T("not detected"), m_joyInfo[1].isPresent ? (LPCTSTR)Format(_T("detected as '%s'"), m_joyInfo[1].caps.szPname) : _T("not detected")));

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

      if (m_joyInfo[0].isPresent) {
        *data = (BYTE)CHANGE_BIT(*data, 0, m_joyInfo[0].XCount > 0);
        *data = (BYTE)CHANGE_BIT(*data, 1, m_joyInfo[0].YCount > 0);
        *data = (BYTE)CHANGE_BIT(*data, 4, !m_joyInfo[0].but1);
        *data = (BYTE)CHANGE_BIT(*data, 5, !m_joyInfo[0].but2);

        if (m_joyInfo[0].XCount > 0) m_joyInfo[0].XCount--;
        if (m_joyInfo[0].YCount > 0) m_joyInfo[0].YCount--;
      }

      if (m_joyInfo[1].isPresent) {
        *data = (BYTE)CHANGE_BIT(*data, 2, m_joyInfo[1].XCount > 0);
        *data = (BYTE)CHANGE_BIT(*data, 3, m_joyInfo[1].YCount > 0);
        *data = (BYTE)CHANGE_BIT(*data, 6, !m_joyInfo[1].but1);
        *data = (BYTE)CHANGE_BIT(*data, 7, !m_joyInfo[1].but2);

        if (m_joyInfo[1].XCount > 0) m_joyInfo[1].XCount--;
        if (m_joyInfo[1].YCount > 0) m_joyInfo[1].YCount--;
      }

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
      updateJoyState(JOYSTICKID1, m_joyInfo[0], true, true);    // update information about
      updateJoyState(JOYSTICKID2, m_joyInfo[1], true, true);    //  both buttons and coordinates.
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
        updateJoyState(JOYSTICKID1, m_joyInfo[0], false, true); // update the button information,
        updateJoyState(JOYSTICKID2, m_joyInfo[1], false, true); //  but not the coordinate data.
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

//
// Reset the joystick state (all capacitors dicharged, all buttons released),
//   and reload calibration data (min/max X-Y information, etc.)
//
void CJoystickCtl::resetJoystick(void) {
  // Refresh joystick calibration data
  updateJoyCaps(JOYSTICKID1, m_joyInfo[0]);
  updateJoyCaps(JOYSTICKID2, m_joyInfo[1]);

  // Pretend the buttons are not pressed
  m_joyInfo[0].but1 = false;
  m_joyInfo[0].but2 = false;
  m_joyInfo[1].but1 = false;
  m_joyInfo[1].but2 = false;

  // Pretend the capacitors have discharged by now, i.e. the counters expired
  m_joyInfo[0].XCount = 0;
  m_joyInfo[0].YCount = 0;
  m_joyInfo[1].XCount = 0;
  m_joyInfo[1].YCount = 0;
}

//
// Retrieves calibration data from the Windows driver
//
MMRESULT CJoystickCtl::updateJoyCaps(
  UINT joyID,           // Identifier of the joystick (JOYSTICKID1 or JOYSTICKID2) to be queried
  JoyInfo& joyInfo )    // Structure to contain the capabilities of the physical joystick
{
  MMRESULT errCode;

  if ((errCode = joyGetDevCaps(joyID, &(joyInfo.caps), sizeof(joyInfo.caps))) != JOYERR_NOERROR) {
    joyInfo.isPresent = false;
  } else {
    joyInfo.isPresent = true;
  }

  return errCode;
}

//
//
//
MMRESULT CJoystickCtl::updateJoyState(
  UINT joyID,           // Identifier of the joystick (JOYSTICKID1 or JOYSTICKID2) to be queried
  JoyInfo& joyInfo,     // Structure to contain the *physical* position and button status of the joystick
  bool doCoords,        // Indicates whether to update *emulated* coordinate information
  bool doButtons )      // Indicates whether to update *emulated* button information
{
  MMRESULT errCode;

  if ((errCode = joyGetPos(joyID, &(joyInfo.state))) != JOYERR_NOERROR) {
    joyInfo.isPresent = false;
  } else {
    joyInfo.isPresent = true;

    if (doCoords) {
      joyInfo.XCount = LIN_REMAP((int)joyInfo.state.wXpos, joyInfo.caps.wXmin, joyInfo.caps.wXmax, m_scaleMin, m_scaleMax);
      joyInfo.YCount = LIN_REMAP((int)joyInfo.state.wYpos, joyInfo.caps.wYmin, joyInfo.caps.wYmax, m_scaleMin, m_scaleMax);
    }

    if (doButtons) {
      joyInfo.but1 = (joyInfo.state.wButtons & JOY_BUTTON1) != 0;
      joyInfo.but2 = (joyInfo.state.wButtons & JOY_BUTTON2) != 0;
    }
  }

  return errCode;
}
