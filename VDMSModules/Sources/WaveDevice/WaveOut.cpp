// WaveOut.cpp : Implementation of CWaveOut
#include "stdafx.h"
#include "WaveDevice.h"
#include "WaveOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_E_OPENDEVICE  _T("Could not open the device '%2' (ID = %1!d!) for playback:%n%n0x%3!08x! - %4%n%nWould you like to try again?")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_BUFOPRANGE    L"buffer"
#define INI_STR_WAVEOUT       L"WaveOut"

/////////////////////////////////////////////////////////////////////////////

#define UM_WOM_ERROR      (WM_USER + 0x100)

/////////////////////////////////////////////////////////////////////////////

#define TOLERANCE_FREQ        10

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CWaveOut

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CWaveOut::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
    &IID_IVDMBasicModule,
    &IID_IWaveDataConsumer
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

STDMETHODIMP CWaveOut::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends    = configuration; // Dependency query object
    Config     = configuration; // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain Wave-Out settings, use defaults if none specified
    m_deviceID = CFG_Get(Config, INI_STR_DEVICEID, -1, 10, false);

    // Try to obtain the buffer operating range (milliseconds), use default if none specified
    m_bufOpRange = CFG_Get(Config, INI_STR_BUFOPRANGE, 125, 10, false);

    /** Get modules ********************************************************/

    // Try to obtain an interface to a Wave-out module, use NULL if none available
    m_waveOut  = DEP_Get(Depends, INI_STR_WAVEOUT, NULL, true);   // do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  m_deviceName = WaveOutGetName();    // Obtain information about the device (its name)

  // Create the garbage-collector thread (manages packets that have finished playing)
  m_gcThread.Create(this, _T("Wave-Out Garbage Collector"), true);      /* TODO: check that creation was successful */
  m_gcThread.SetPriority(THREAD_PRIORITY_LOWEST);
  m_gcThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  // Release the Wave device
  if (m_hWaveOut != NULL)
    WaveOutClose();

  // Signal the GC thread to quit
  if (m_gcThread.GetThreadHandle() != NULL)
    m_gcThread.Cancel();

  // Release the Wave-out module
  m_waveOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IWaveDataConsumer
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CWaveOut::SetFormat(WORD channels, DWORD samplesPerSec, WORD bitsPerSample) {
  /* TODO: validate that 10% tolerance in frequency chane is worth anything; if yes, make it parametrizable
     in VDMS.ini file as a number n = 0...100, then use tolerance range = 1.00 +/- (n / 100.0) */

  HRESULT hrThis = S_OK, hrThat = S_OK;

  // Check if the device has to be (re)opened either because it was not
  //  opened before, or following a change in playback format that requires
  //  the device to be closed then opened again with the new format
  if ((m_hWaveOut == NULL) ||
      (m_waveFormat.nChannels != channels) ||
      (m_waveFormat.nSamplesPerSec * (1.00 - TOLERANCE_FREQ / 100.0) > samplesPerSec) || // accept 10% tolerance in freq. change, avoids excessive overhead in
      (m_waveFormat.nSamplesPerSec * (1.00 + TOLERANCE_FREQ / 100.0) < samplesPerSec) || // closing/re-opening the device (some games abuse the time-constant)
      (m_waveFormat.wBitsPerSample != bitsPerSample))
  {
    MMRESULT errCode;

    // If the device is open, try to close it first
    while (m_hWaveOut != NULL) {
      switch (errCode = waveOutClose(m_hWaveOut)) {
        case MMSYSERR_NOERROR:
          m_hWaveOut = NULL;
          break;
        case WAVERR_STILLPLAYING:
          Sleep(15);
          break;
        default:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("waveOutClose(0x%08x) on device %d ('%s'): 0x%08x - %s"), (int)m_hWaveOut, (int)m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
          hrThis = S_FALSE;
      }
    }

    // (Re)open the (now closed) device
    if (hrThis == S_OK) {
      // Change the format
      m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
      m_waveFormat.nChannels = channels;
      m_waveFormat.nSamplesPerSec = samplesPerSec;
      m_waveFormat.nAvgBytesPerSec = channels * samplesPerSec * bitsPerSample / 8;
      m_waveFormat.nBlockAlign = channels * bitsPerSample / 8;
      m_waveFormat.wBitsPerSample = bitsPerSample;
      m_waveFormat.cbSize = 0;

      // Open the device with the new format
      if (!WaveOutOpen(false))
        hrThis = S_FALSE;

      // No bytes are enqueued for playback (yet)
      InterlockedExchange(&m_bufferedBytes, 0);

      // Compute the ideal amount of buffering (in bytes)
      m_bufferedLo = m_waveFormat.nAvgBytesPerSec * m_bufOpRange / 1000;
      m_bufferedHi = m_bufferedLo * 2;
    }
  }

  if (m_waveOut != NULL)
    hrThat = m_waveOut->SetFormat(channels, samplesPerSec, bitsPerSample);

  return (hrThat == S_OK ? hrThis : hrThat);
}

STDMETHODIMP CWaveOut::PlayData(BYTE * data, LONG length, DOUBLE * load) {
  if (data == NULL)
    return E_POINTER;

  if (load == NULL)
    return E_POINTER;

  HRESULT hrThis = S_OK, hrThat = S_OK;
  DOUBLE loadThis = 1.0, loadThat = 1.0;

  if ((m_hWaveOut == NULL) && (!WaveOutOpen())) {
    hrThis = S_FALSE;         // The device is not open, and an attempt to open it failed
  } else {
    WAVEHDR* waveHdr = NULL;
    LPSTR waveData = NULL;

    try {

      /* TODO: get rid of this silly new/delete scheme; replace with static,
         pre-allocated circular buffer (size specifiable in VDMS.ini?) and
         pre-allocated WAVEHDR's that point at consecutive locations in the
         buffer -- new/delete is too inefficient (we can make use of the fact
         that we know that data will only be allocated/released in a FIFO
         manner) */

      // Allocate header & data needed by Windows to play back a sound-block
      waveHdr  = new WAVEHDR;
      waveData = new CHAR[length];

      // Fill in the header information
      waveHdr->lpData = waveData;
      waveHdr->dwBufferLength = length;
      waveHdr->dwBytesRecorded = 0;
      waveHdr->dwUser = (DWORD)waveHdr;
      waveHdr->dwFlags = 0;
      waveHdr->dwLoops = 0;

      // Copy the data to the playback buffer
      memcpy(waveData, data, length);

      // Prepare and send the header + data to the Windows device

      MMRESULT errCode;

      if ((errCode = waveOutPrepareHeader(m_hWaveOut, waveHdr, sizeof(*waveHdr))) != MMSYSERR_NOERROR) {
        CString args = Format(_T("0x%08x, %p, %d"), m_hWaveOut, waveHdr, sizeof(*waveHdr));
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("waveOutPrepareHeader(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
        AfxThrowUserException();
      }

      if ((errCode = waveOutWrite(m_hWaveOut, waveHdr, sizeof(*waveHdr))) != MMSYSERR_NOERROR) {
        CString args = Format(_T("0x%08x, %p, %d"), m_hWaveOut, waveHdr, sizeof(*waveHdr));
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("waveOutWrite(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
        AfxThrowUserException();
      }

      // The data was enqueued successfully => update buffer byte count
      LONG lastBufferedBytes = InterlockedExchangeAdd(&m_bufferedBytes, length);

      if (lastBufferedBytes < m_bufferedLo) {
        loadThis = (double)lastBufferedBytes / m_bufferedLo;
      } else if (lastBufferedBytes > m_bufferedHi) {
        loadThis = (double)lastBufferedBytes / m_bufferedHi;
      }
    } catch (CMemoryException * pme) {
      TCHAR errMsg[1024] = _T("<no description available>");
      pme->GetErrorMessage(errMsg, sizeof(errMsg)/sizeof(errMsg[0]));
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("An unexpected out-of-memory condition was encountered while handling a Wave system-exclusive message:\nwaveHdr  = %p (%d bytes)\nwaveData= %p (%d bytes)\n%s"), waveHdr, sizeof(*waveHdr), waveData, length + 2, errMsg));
      delete waveHdr;
      delete[] waveData;
      hrThis = S_FALSE;
    } catch (CUserException * /*pue*/) {
      delete waveHdr;
      delete[] waveData;
      hrThis = S_FALSE;
    }
  }

  // Forward the call to other module(s) daisy-chained after us (if any)
  if (m_waveOut != NULL)
    hrThat = m_waveOut->PlayData(data, length, &loadThat);

  // Decide the load (compromise between us and other modules)
  *load = loadThis * loadThat;

  return (hrThat == S_OK ? hrThis : hrThat);
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CWaveOut::Run(CThread& thread) {
  MSG message;

  _ASSERTE(thread.GetThreadID() == m_gcThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  while (thread.GetMessage(&message)) {
    HWAVEOUT hWaveOut = (HWAVEOUT)(message.wParam);
    WAVEHDR* waveHdr = (WAVEHDR*)(message.lParam);
    MMRESULT errCode = (MMRESULT)(message.lParam);

    switch (message.message) {
      case WM_QUIT:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread cancelled")));
        return 0;

      case MM_WOM_OPEN:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Wave-out device opened (handle = 0x%08x)"), hWaveOut));
        break;

      case MM_WOM_CLOSE:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Wave-out device closed (handle = 0x%08x)"), hWaveOut));
        break;

      case MM_WOM_DONE:
        _ASSERTE(waveHdr != NULL);
        _ASSERTE(waveHdr->lpData != NULL);
        _ASSERTE(waveHdr->dwUser != NULL);
        _ASSERTE((waveHdr->dwFlags & WHDR_DONE) == WHDR_DONE);

        delete[] (CHAR*)(waveHdr->lpData);
        delete (WAVEHDR*)(waveHdr->dwUser);
        break;

      case UM_WOM_ERROR:
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("WaveOut (hWaveOut = 0x%08x) error on device %d ('%s'): 0x%08x - %s"), hWaveOut, m_deviceID, (LPCTSTR)(m_deviceName), (int)errCode, (LPCTSTR)(WaveOutGetError(errCode))));
        break;

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
// Callback function invoked by WAVE driver when the device is opened/closed,
//  or when a wave "packet" finishes playing.
//
void CALLBACK CWaveOut::WaveOutProc(HWAVEOUT hwo, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
  MMRESULT errCode;
  CWaveOut* pThis = (CWaveOut*)dwInstance;
  WAVEHDR* waveHdr = (WAVEHDR*)(dwParam1);

  _ASSERTE(pThis != NULL);

  try {
    switch (wMsg) {
      case WOM_OPEN:
        pThis->m_gcThread.PostMessage(MM_WOM_OPEN, (WPARAM)hwo, NULL);
        break;
      case WOM_CLOSE:
        pThis->m_gcThread.PostMessage(MM_WOM_CLOSE, (WPARAM)hwo, NULL);
        break;
      case WOM_DONE:
        _ASSERTE(hwo == pThis->m_hWaveOut);
        _ASSERTE(waveHdr != NULL);

        InterlockedExchangeAdd(&(pThis->m_bufferedBytes), -(LONG)(waveHdr->dwBufferLength));

        if ((errCode = waveOutUnprepareHeader(hwo, waveHdr, sizeof(*waveHdr))) != MMSYSERR_NOERROR) {
          pThis->m_gcThread.PostMessage(UM_WOM_ERROR, (WPARAM)hwo, (LPARAM)errCode);
        } else {
          pThis->m_gcThread.PostMessage(MM_WOM_DONE, (WPARAM)hwo, (LPARAM)dwParam1);
        } break;
    }
  } catch (...) { }
}

//
// Opens the Wave device specified in the module's settings
//
bool CWaveOut::WaveOutOpen(bool isInteractive) {
  static bool isErrPrompt = true;
  static bool isErrLog    = true;
  static time_t lastRetry = 0;

  // Don't open the device if it's already open
  if (m_hWaveOut != NULL)
    return true;

  // Only attempt to open the device at reasonable intervals, to avoid excessive overhead
  if ((time(NULL) - lastRetry) < WAVEOPEN_RETRY_INTERVAL)
    return false;           // another attempt to open the device was made very recently; don't overdo it

  MMRESULT errCode;

  // Attempt to open the Wave-out device
  while ((errCode = waveOutOpen(&m_hWaveOut, m_deviceID, &m_waveFormat, (DWORD)WaveOutProc, (DWORD)(this), CALLBACK_FUNCTION)) != MMSYSERR_NOERROR) {
    if (isErrLog) {         // do we log the error ?
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not open the device %d ('%s'):\n0x%08x - %s"), m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
      isErrLog = false;     // only log this error once, as it will probably (re)occur several times in a row
    }

    if (!isInteractive)     // do we want to pop up a message box ?
      break;                // stop trying

    if (!isErrPrompt)       // did the user select not to be prompted again ?
      break;                // stop trying

    if (MessageBox(FormatMessage(MSG_ERR_E_OPENDEVICE, false, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)),
                   _T("Wave-out Device Error"), /* TODO: LoadString(...) */
                   MB_RETRYCANCEL, MB_ICONERROR) == IDCANCEL)
    {
      isErrPrompt = false;  // the user does not whish to be prompted again !
      break;                // stop trying
    }
  }

  if (m_hWaveOut != NULL) { // The device was opened successfully
    isErrLog    = true;     // Next time we get an error, log it
    isErrPrompt = true;     // Next time we get an error, prompt the user (if interactive)
    lastRetry   = 0;        // If the device is closed and reopened within WAVEOPEN_RETRY_INTERVAL, don't complain
    return true;
  } else {                  // The device could not be open, and the user did not retry
    lastRetry = time(NULL); // Remember not to try to reopen the device within WAVEOPEN_RETRY_INTERVAL from a failure
    return false;
  }
}

//
// Closes the Wave device specified in the module's settings
//
void CWaveOut::WaveOutClose(void) {
  waveOutReset(m_hWaveOut);   // Silence the device
  waveOutClose(m_hWaveOut);   // Close the device
  m_hWaveOut = NULL;          // Lose the handle
}

//
//
//
CString CWaveOut::WaveOutGetName(void) {
  MMRESULT errCode;
  WAVEOUTCAPS waveCaps;

  if ((errCode = waveOutGetDevCaps(m_deviceID, &waveCaps, sizeof(waveCaps))) != MMSYSERR_NOERROR) {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not obtain more information about the device %d:\n0x%08x - %s"), m_deviceID, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
    return CString(_T("<unknown>"));
  } else {
    return CString(waveCaps.szPname);
  }
}

//
//
//
CString CWaveOut::WaveOutGetError(MMRESULT errCode) {
  TCHAR buf[1024] = _T("<unknown>");
  waveOutGetErrorText(errCode, buf, sizeof(buf) / sizeof(buf[0]));
  return CString(buf);
}
