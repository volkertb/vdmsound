// MIDIIn.cpp : Implementation of CMIDIIn
#include "stdafx.h"
#include "MIDIDevice.h"
#include "MIDIIn.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_E_OPENDEVICE  _T("Could not open the device '%2' (ID = %1!d!) for playback:%n%n0x%3!08x! - %4%n%nWould you like to try again?")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_MIDIOUT       L"MidiOut"

/////////////////////////////////////////////////////////////////////////////

#define UM_MIM_TIMESTAMP  (WM_USER + 0x100)

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

#define _MIM_STATUS(x) ((BYTE)(((x) >>  0) & 0x000000ff))
#define _MIM_DATA1(x)  ((BYTE)(((x) >>  8) & 0x000000ff))
#define _MIM_DATA2(x)  ((BYTE)(((x) >> 16) & 0x000000ff))

int MIDI_evt_len[256] = {
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x00
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x10
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x20
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x30
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x40
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x50
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x60
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x70

  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0x80
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0x90
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xa0
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xb0

  2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2,  // 0xc0
  2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2,  // 0xd0

  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xe0

  2,0,3,2, 0,0,1,1, 1,0,1,1, 1,0,1,1   // 0xf0
};

/////////////////////////////////////////////////////////////////////////////
// CMIDIIn

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIIn::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule
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

STDMETHODIMP CMIDIIn::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends = configuration;    // Dependency query object
    Config  = configuration;    // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain the MIDI-In settings, use defaults if none specified
    m_deviceID = CFG_Get(Config, INI_STR_DEVICEID, -1, 10, false);

    /** Get modules ********************************************************/

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut  = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, false);
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  m_deviceName = MidiInGetName();     // Obtain information about the device (its name)

  // Create the garbage-collector thread (frees up memory allocated during asynchronous MIDI SysEx uploads)
  m_recThread.Create(this, _T("MIDI-In Recorder"), true);      /* TODO: check that creation was successful */
  m_recThread.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
  m_recThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIIn initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

  return S_OK;
}

STDMETHODIMP CMIDIIn::Destroy() {
  // Release the MIDI device
  if (m_hMidiIn != NULL)
    MidiInClose();

  // Signal the GC thread to quit
  if (m_recThread.GetThreadHandle() != NULL)
    m_recThread.Cancel();

  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIIn released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CMIDIIn::Run(CThread& thread) {
  MSG message;
  DWORD thisTime = 0, lastTime = 0;
  bool sentLastEvent = true;

  _ASSERTE(thread.GetThreadID() == m_recThread.GetThreadID());

  MidiInOpen();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Recorder thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  while (thread.GetMessage(&message)) {
    HMIDIIN  hMidiIn = (HMIDIIN)(message.wParam);
    MIDIHDR* midiHdr = (MIDIHDR*)(message.lParam);
    DWORD    midiMsg = (DWORD)(message.lParam);

    BYTE     status  = _MIM_STATUS(midiMsg);
    BYTE     data1   = _MIM_DATA1(midiMsg);
    BYTE     data2   = _MIM_DATA2(midiMsg);
    int      length  = MIDI_evt_len[status & 0xff];

    switch (message.message) {
      case WM_QUIT:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Recorder thread cancelled")));
        return 0;

      case MM_MIM_OPEN:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDI-in device opened (handle = 0x%08x)"), hMidiIn));
        break;

      case MM_MIM_CLOSE:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDI-in device closed (handle = 0x%08x)"), hMidiIn));
        break;

      case UM_MIM_TIMESTAMP:
        lastTime = lastTime == 0 ? message.lParam : sentLastEvent ? thisTime : lastTime;
        thisTime = message.lParam;
        break;

      case MM_MIM_DATA:
        if (m_midiOut != NULL) {
          HRESULT hr;

          sentLastEvent = false;

          if (length < 1) {
            RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid MIDI event received from driver: %d %d %d (%08x)"), status & 0xff, data1 & 0xff, data2 & 0xff, (int)midiMsg));
            break;
          }

          if (FAILED(hr = m_midiOut->HandleEvent((thisTime - lastTime) * 1000l, status, data1, data2, length - 1))) {
            RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not push MIDI event:\n0x%08x - %s"), (int)hr, (LPCTSTR)FormatMessage(hr)));
          } else {
            sentLastEvent = true;
          }
        } break;

      case MM_MIM_ERROR:
        if (m_midiOut != NULL) {
          sentLastEvent = false;
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_WARNING, Format(_T("Invalid MIDI event received from external device: %d %d %d (%08x)"), status & 0xff, data1 & 0xff, data2 & 0xff, (int)midiMsg));
        } break;

      default:
        break;
    }
  }

  DWORD lastError = GetLastError();
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Abnormal condition encountered while waiting on message queue:\n0x%08x - %s"), lastError, (LPCTSTR)FormatMessage(lastError)));

  return -2;  // abnormal thread termination (error in message fetch)
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
// Callback function invoked by MIDI driver when the device is opened/closed,
//  or when a MIDI event (or SysEx) is received.  Necessary to obtain the
//  MIDI event timestamp (can only be obtained through callback method).
//
void CALLBACK CMIDIIn::MidiInProc(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
  CMIDIIn* pThis = (CMIDIIn*)dwInstance;

  _ASSERTE(pThis != NULL);

  try {
    switch (wMsg) {
      case MIM_OPEN:
        pThis->m_recThread.PostMessage(MM_MIM_OPEN, (WPARAM)hmi, NULL);
        break;
      case MIM_CLOSE:
        pThis->m_recThread.PostMessage(MM_MIM_CLOSE, (WPARAM)hmi, NULL);
        break;
      case MIM_DATA:
      case MIM_LONGDATA:
      case MIM_ERROR:
      case MIM_LONGERROR:
#       if ((MIM_DATA != MM_MIM_DATA) || (MIM_ERROR != MM_MIM_ERROR) || (MIM_LONGDATA != MM_MIM_LONGDATA) || (MIM_LONGERROR != MM_MIM_LONGERROR))
#       error Incosistency between MIM_xxx and MM_MIM_xxx macros, will result in abnormal runtime behaviour!
#       endif
        _ASSERTE(hmi == pThis->m_hMidiIn);
        pThis->m_recThread.PostMessage(UM_MIM_TIMESTAMP, (WPARAM)hmi, dwParam2);
        pThis->m_recThread.PostMessage(wMsg, (WPARAM)hmi, dwParam1);
        break;
    }
  } catch (...) { }
}

//
// Opens the MIDI device specified in the module's settings
//
bool CMIDIIn::MidiInOpen(bool isInteractive) {
  static bool isErrPrompt = true;
  static bool isErrLog    = true;
  static time_t lastRetry = 0;

  // Don't open the device if it's already open
  if (m_hMidiIn != NULL)
    return true;

  // Only attempt to open the device at reasonable intervals, to avoid excessive overhead
  if ((time(NULL) - lastRetry) < MIDIIN_OPEN_RETRY_INTERVAL) {
    return false;           // another attempt to open the device was made very recently; don't overdo it
  } else {
    lastRetry = time(NULL);
  }

  MMRESULT errCode;

  // Attempt to open the MIDI-in device
  while ((errCode = midiInOpen(&m_hMidiIn, m_deviceID, m_recThread.GetThreadID(), NULL, CALLBACK_THREAD)) != MMSYSERR_NOERROR) {
    if (isErrLog) {         // do we log the error ?
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not open the device %d ('%s'):\n0x%08x - %s"), m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiInGetError(errCode)));
      isErrLog = false;     // only log this error once, as it will probably (re)occur several times in a row
    }

    if (!isInteractive)     // do we want to pop up a message box ?
      break;                // stop trying

    if (!isErrPrompt)       // did the user select not to be prompted again ?
      break;                // stop trying

    if (MessageBox(FormatMessage(MSG_ERR_E_OPENDEVICE, false, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiInGetError(errCode)),
                   _T("MIDI-in Device Error"), /* TODO: LoadString(...) */
                   MB_RETRYCANCEL, MB_ICONERROR) == IDCANCEL)
    {
      isErrPrompt = false;  // the user does not whish to be prompted again !
      break;                // stop trying
    }
  }

  if (m_hMidiIn != NULL) {  // The device was opened successfully
    isErrLog    = true;     // Next time we get an error, log it
    isErrPrompt = true;     // Next time we get an error, prompt the user (if interactive)

    midiInStart(m_hMidiIn); // start recording

    return true;
  } else {                  // The device could not be open, and the user did not retry
    return false;
  }
}

//
// Closes the MIDI device specified in the module's settings
//
void CMIDIIn::MidiInClose(void) {
  midiInStop(m_hMidiIn);      // stop recording
  midiInReset(m_hMidiIn);     // Silence the device
  midiInClose(m_hMidiIn);     // Close the device
  m_hMidiIn = NULL;           // Lose the handle
}

//
//
//
CString CMIDIIn::MidiInGetName(void) {
  MMRESULT errCode;
  MIDIINCAPS midiCaps;

  if ((errCode = midiInGetDevCaps(m_deviceID, &midiCaps, sizeof(midiCaps))) != MMSYSERR_NOERROR) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not obtain more information about the device %d:\n0x%08x - %s"), m_deviceID, (int)errCode, (LPCTSTR)MidiInGetError(errCode)));
    return CString(_T("<unknown>"));
  } else {
    return CString(midiCaps.szPname);
  }
}

//
//
//
CString CMIDIIn::MidiInGetError(MMRESULT errCode) {
  TCHAR buf[1024] = _T("<unknown>");
  midiInGetErrorText(errCode, buf, sizeof(buf) / sizeof(buf[0]));
  return CString(buf);
}
