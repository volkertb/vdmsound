// MIDIOut.cpp : Implementation of CMIDIOut
#include "stdafx.h"
#include "MIDIDevice.h"
#include "MIDIOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_E_OPENDEVICE  _T("Could not open the device '%2' (ID = %1!d!) for playback:%n%n0x%3!08x! - %4%n%nWould you like to try again?")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_MIDIOUT       L"MidiOut"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CMIDIOut

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIOut::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IVDMBasicModule,
    &IID_IMIDIEventHandler
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

STDMETHODIMP CMIDIOut::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Obtain the Query objects (for intialization purposes)
  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends(configuration);  // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config(configuration);  // Configuration query object

  try {
    // Obtain MIDI-Out settings (if available)
    m_deviceID = CFG_Get(Config, INI_STR_DEVICEID, -1, 10, false);

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut  = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, true);   // do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  m_deviceName = MidiOutGetName();    // Obtain information about the device (its name)

  // Create the garbage-collector thread (frees up memory allocated during asynchronous MIDI SysEx uploads)
  gcThread.Create(this, true);
  gcThread.SetPriority(THREAD_PRIORITY_LOWEST);
  gcThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

  return S_OK;
}

STDMETHODIMP CMIDIOut::Destroy() {
  // Release the MIDI device
  if (m_hMidiOut != NULL)
    MidiOutClose();

  // Signal the GC thread to quit
  if (gcThread.GetThreadHandle() != NULL)
    gcThread.Cancel();

  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIOut released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIOut::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  if (m_hMidiOut == NULL)
    MidiOutOpen();

  union {
    DWORD dwData;
    BYTE bData[4];
  } data;

  data.bData[0] = status;   // MIDI status byte
  data.bData[1] = data1;    // first MIDI data byte
  data.bData[2] = data2;    // second MIDI data byte
  data.bData[3] = 0;

  midiOutShortMsg(m_hMidiOut, data.dwData);

  if (m_midiOut != NULL)
    return m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return S_OK;
}

STDMETHODIMP CMIDIOut::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
  if (data == NULL)
    return E_POINTER;

  if (m_hMidiOut == NULL)
    MidiOutOpen();

  MIDIHDR* midiHdr = NULL;
  LPSTR sysExMsg = NULL;

  try {
    midiHdr  = new MIDIHDR;
    sysExMsg = new CHAR[length + 2];

    sysExMsg[0] = (char)(0xf0);           // SYSEX event
    memcpy(&(sysExMsg[1]), data, length); // the actual SysEx data
    sysExMsg[length + 1] = (char)(0xf7);  // EOX event

    midiHdr->lpData = sysExMsg;
    midiHdr->dwBufferLength = length + 2;
    midiHdr->dwBytesRecorded = 0;
    midiHdr->dwUser = (DWORD)midiHdr;
    midiHdr->dwFlags = 0;

    MMRESULT errCode;

    if ((errCode = midiOutPrepareHeader(m_hMidiOut, midiHdr, sizeof(*midiHdr))) != MMSYSERR_NOERROR) {
      CString args = Format(_T("0x%08x, %p, %d"), m_hMidiOut, midiHdr, sizeof(*midiHdr));
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("midiOutPrepareHeader(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiOutGetError(errCode)));
      AfxThrowUserException();
    }

    if ((errCode = midiOutLongMsg(m_hMidiOut, midiHdr, sizeof(*midiHdr))) != MMSYSERR_NOERROR) {
      CString args = Format(_T("0x%08x, %p, %d"), m_hMidiOut, midiHdr, sizeof(*midiHdr));
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("midiOutLongMsg(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiOutGetError(errCode)));
      AfxThrowUserException();
    }
  } catch (CMemoryException * pme) {
    TCHAR errMsg[1024] = _T("<no description available>");
    pme->GetErrorMessage(errMsg, sizeof(errMsg)/sizeof(errMsg[0]));
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("An unexpected out-of-memory condition was encountered while handling a MIDI system-exclusive message:\nmidiHdr  = %p (%d bytes)\nsysExMsg = %p (%d bytes)\n%s"), midiHdr, sizeof(*midiHdr), sysExMsg, length + 2, errMsg));
    delete midiHdr;
    delete[] sysExMsg;
  } catch (CUserException * /*pue*/) {
    delete midiHdr;
    delete[] sysExMsg;
  }

  if (m_midiOut != NULL)
    return m_midiOut->HandleSysEx(usDelta, data, length);

  return S_OK;
}

STDMETHODIMP CMIDIOut::HandleRealTime(LONGLONG usDelta, BYTE data) {
  if (m_midiOut != NULL)
    return m_midiOut->HandleRealTime(usDelta, data);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CMIDIOut::Run(CThread& thread) {
  MSG message;

  _ASSERTE(thread.GetThreadID() == gcThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  while (thread.GetMessage(&message)) {
    MMRESULT errCode;
    HMIDIOUT hMidiOut = (HMIDIOUT)(message.wParam);
    MIDIHDR* midiHdr = (MIDIHDR*)(message.lParam);

    switch (message.message) {
      case WM_QUIT:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread cancelled")));
        return 0;

      case MM_MOM_OPEN:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDI-out device opened (handle = 0x%08x)"), hMidiOut));
        break;

      case MM_MOM_CLOSE:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDI-out device closed (handle = 0x%08x)"), hMidiOut));
        break;

      case MM_MOM_DONE:
        _ASSERTE(hMidiOut == m_hMidiOut);
        _ASSERTE(midiHdr != NULL);
        _ASSERTE(midiHdr->lpData != NULL);
        _ASSERTE(midiHdr->dwUser != NULL);
        _ASSERTE((midiHdr->dwFlags & MHDR_DONE) == MHDR_DONE);

        if ((errCode = midiOutUnprepareHeader(hMidiOut, midiHdr, sizeof(*midiHdr))) != MMSYSERR_NOERROR) {
          CString args = Format(_T("0x%08x, %p, %d"), hMidiOut, midiHdr, sizeof(*midiHdr));
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("midiOutUnprepareHeader(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)(m_deviceName), (int)errCode, (LPCTSTR)(MidiOutGetError(errCode))));
        }

        delete[] (CHAR*)(midiHdr->lpData);
        delete (MIDIHDR*)(midiHdr->dwUser);
        break;

      default:
        break;
    }
  }
  
  DWORD lastError = GetLastError();
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Abnormal condition encoundered while waiting on message queue:\n0x%08x - %s"), lastError, (LPCTSTR)FormatMessage(lastError)));

  return -2;  // abnormal thread termination (error in message fetch)
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

//
// Opens the MIDI device specified in the module's settings
//
void CMIDIOut::MidiOutOpen(void) {
  static bool isErrPrompt = true;
  static time_t lastRetry = 0;

  // Only attempt to open the device at reasonable intervals, to avoid excessive overhead
  if ((time(NULL) - lastRetry) < MIDIOPEN_RETRY_INTERVAL) {
    return;   // another attempt to open the device was made very recently; don't overdo it
  } else {
    lastRetry = time(NULL);
  }

  // Don't open the device if it's already open
  if (m_hMidiOut != NULL)
    return;

  MMRESULT errCode;

  // Attempt to open the MIDI-out device
  while ((errCode = midiOutOpen(&m_hMidiOut, m_deviceID, gcThread.GetThreadID(), (DWORD)(this), CALLBACK_THREAD)) != MMSYSERR_NOERROR) {
    if (!isErrPrompt)       // did the user select not to be prompted again ?
      break;                // stop trying

    if (MessageBox(FormatMessage(MSG_ERR_E_OPENDEVICE, false, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiOutGetError(errCode)),
                   _T("MIDI-out Device Error"), /* TODO: LoadString(...) */
                   MB_RETRYCANCEL, MB_ICONERROR) == IDCANCEL)
    {
      isErrPrompt = false;  // the user does not whish to be prompted again !
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not open the device %d ('%s'):\n0x%08x - %s"), m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)MidiOutGetError(errCode)));
      break;                // stop trying
    }
  }
}

//
// Closes the MIDI device specified in the module's settings
//
void CMIDIOut::MidiOutClose(void) {
  midiOutReset(m_hMidiOut);   // Silence the device
  midiOutClose(m_hMidiOut);   // Close the device
  m_hMidiOut = NULL;          // Lose the handle
}

//
//
//
CString CMIDIOut::MidiOutGetName(void) {
  MMRESULT errCode;
  MIDIOUTCAPS midiCaps;

  if ((errCode = midiOutGetDevCaps(m_deviceID, &midiCaps, sizeof(midiCaps))) != MMSYSERR_NOERROR) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not obtain more information about the device %d:\n0x%08x - %s"), m_deviceID, (int)errCode, (LPCTSTR)MidiOutGetError(errCode)));
    return CString(_T("<unknown>"));
  } else {
    return CString(midiCaps.szPname);
  }
}

//
//
//
CString CMIDIOut::MidiOutGetError(MMRESULT errCode) {
  TCHAR buf[1024] = _T("<unknown>");
  midiOutGetErrorText(errCode, buf, sizeof(buf) / sizeof(buf[0]));
  return CString(buf);
}
