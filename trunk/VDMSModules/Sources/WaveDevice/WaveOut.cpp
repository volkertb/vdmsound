// WaveOut.cpp : Implementation of CWaveOut
#include "stdafx.h"
#include "WaveDevice.h"
#include "WaveOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_E_OPENDEVICE  _T("Could not open the device '%2' (ID = %1!d!) for playback:%n%n0x%3!08x! - %4%n%nWould you like to try again?")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_WAVEOUT       L"WaveOut"

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

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Obtain the Query objects (for intialization purposes)
  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends(configuration);  // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config(configuration);  // Configuration query object

  try {
    // Obtain Wave-Out settings (if available)
    m_deviceID = CFG_Get(Config, INI_STR_DEVICEID, -1, 10, false);

    // Try to obtain an interface to a Wave-out module, use NULL if none available
    m_waveOut  = DEP_Get(Depends, INI_STR_WAVEOUT, NULL, true);   // do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  m_deviceName = WaveOutGetName();    // Obtain information about the device (its name)

  // Create the garbage-collector thread (manages packets that have finished playing)
  gcThread.Create(this, true);
  gcThread.SetPriority(THREAD_PRIORITY_LOWEST);
  gcThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  // Release the Wave device
  if (m_hWaveOut != NULL)
    WaveOutClose();

  // Signal the GC thread to quit
  if (gcThread.GetThreadHandle() != NULL)
    gcThread.Cancel();

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
  if ((m_waveFormat.nChannels != channels) ||
      (m_waveFormat.nSamplesPerSec != samplesPerSec) ||
      (m_waveFormat.wBitsPerSample != bitsPerSample))
  {
    MMRESULT errCode;

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
          return S_FALSE;
      }
    }

    m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_waveFormat.nChannels = channels;
    m_waveFormat.nSamplesPerSec = samplesPerSec;
    m_waveFormat.nAvgBytesPerSec = channels * samplesPerSec * bitsPerSample / 8;
    m_waveFormat.nBlockAlign = channels * bitsPerSample / 8;
    m_waveFormat.wBitsPerSample = bitsPerSample;
    m_waveFormat.cbSize = 0;

    WaveOutOpen();
  }

  return S_OK;
}

STDMETHODIMP CWaveOut::PlayData(BYTE * data, LONG length) {
  if (data == NULL)
    return E_POINTER;

  if (m_hWaveOut == NULL)
    WaveOutOpen();

  WAVEHDR* waveHdr = NULL;
  LPSTR waveData = NULL;

  try {

    /* TODO: get rid of this silly new/delete scheme; replace with static,
       pre-allocated circular buffer (size specifiable in VDMS.ini?) and
       pre-allocated WAVEHDR's that point at consecutive locations in the
       buffer -- new/delete is too inefficient */

    waveHdr  = new WAVEHDR;
    waveData = new CHAR[length];

    memcpy(waveData, data, length); // the actual data

    waveHdr->lpData = waveData;
    waveHdr->dwBufferLength = length;
    waveHdr->dwBytesRecorded = 0;
    waveHdr->dwUser = (DWORD)waveHdr;
    waveHdr->dwFlags = 0;
    waveHdr->dwLoops = 0;

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
  } catch (CMemoryException * pme) {
    TCHAR errMsg[1024] = _T("<no description available>");
    pme->GetErrorMessage(errMsg, sizeof(errMsg)/sizeof(errMsg[0]));
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("An unexpected out-of-memory condition was encountered while handling a Wave system-exclusive message:\nwaveHdr  = %p (%d bytes)\nwaveData= %p (%d bytes)\n%s"), waveHdr, sizeof(*waveHdr), waveData, length + 2, errMsg));
    delete waveHdr;
    delete[] waveData;
  } catch (CUserException * /*pue*/) {
    delete waveHdr;
    delete[] waveData;
  }

  if (m_waveOut != NULL)
    return m_waveOut->PlayData(data, length);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IRunnable
/////////////////////////////////////////////////////////////////////////////

unsigned int CWaveOut::Run(CThread& thread) {
  MSG message;

  _ASSERTE(thread.GetThreadID() == gcThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  while (thread.GetMessage(&message)) {
    MMRESULT errCode;
    HWAVEOUT hWaveOut = (HWAVEOUT)(message.wParam);
    WAVEHDR* waveHdr = (WAVEHDR*)(message.lParam);

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
        _ASSERTE(hWaveOut == m_hWaveOut);
        _ASSERTE(waveHdr != NULL);
        _ASSERTE(waveHdr->lpData != NULL);
        _ASSERTE(waveHdr->dwUser != NULL);
        _ASSERTE((waveHdr->dwFlags & MHDR_DONE) == MHDR_DONE);

        if ((errCode = waveOutUnprepareHeader(hWaveOut, waveHdr, sizeof(*waveHdr))) != MMSYSERR_NOERROR) {
          CString args = Format(_T("0x%08x, %p, %d"), hWaveOut, waveHdr, sizeof(*waveHdr));
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("waveOutUnprepareHeader(%s) on device %d ('%s'): 0x%08x - %s"), (LPCTSTR)args, m_deviceID, (LPCTSTR)(m_deviceName), (int)errCode, (LPCTSTR)(WaveOutGetError(errCode))));
        }

        delete[] (CHAR*)(waveHdr->lpData);
        delete (WAVEHDR*)(waveHdr->dwUser);
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
// Opens the Wave device specified in the module's settings
//
void CWaveOut::WaveOutOpen(void) {
  static bool isErrPrompt = true;
  static time_t lastRetry = 0;

  // Only attempt to open the device at reasonable intervals, to avoid excessive overhead
  if ((time(NULL) - lastRetry) < WAVEOPEN_RETRY_INTERVAL) {
    return;   // another attempt to open the device was made very recently; don't overdo it
  } else {
    lastRetry = time(NULL);
  }

  // Don't open the device if it's already open
  if (m_hWaveOut != NULL)
    return;

  MMRESULT errCode;

  // Attempt to open the Wave-out device
  while ((errCode = waveOutOpen(&m_hWaveOut, m_deviceID, &m_waveFormat, gcThread.GetThreadID(), (DWORD)(this), CALLBACK_THREAD)) != MMSYSERR_NOERROR) {
    if (!isErrPrompt)       // did the user select not to be prompted again ?
      break;                // stop trying

    if (MessageBox(FormatMessage(MSG_ERR_E_OPENDEVICE, false, m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)),
                   _T("Wave-out Device Error"), /* TODO: LoadString(...) */
                   MB_RETRYCANCEL, MB_ICONERROR) == IDCANCEL)
    {
      isErrPrompt = false;  // the user does not whish to be prompted again !
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not open the device %d ('%s'):\n0x%08x - %s"), m_deviceID, (LPCTSTR)m_deviceName, (int)errCode, (LPCTSTR)WaveOutGetError(errCode)));
      break;                // stop trying
    }
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
