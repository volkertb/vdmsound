// WaveOut.cpp : Implementation of CWaveOut
#include "stdafx.h"
#include "DSoundDevice.h"
#include "WaveOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_E_OPENDEVICE  _T("Could not open the device '%2' (ID = %1!d!) for playback:%n%n0x%3!08x! - %4%n%nWould you like to try again?")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_BUFOPRANGE    L"buffer"
#define INI_STR_WAVEOUT       L"WaveOut"

/////////////////////////////////////////////////////////////////////////////

#define TOLERANCE_FREQ        10  // how much % change in frequency does not justify re-creating the DSound buffer due to format change
#define BUF_CHUNKS            8   // how big the DSound buffer should be as a multiple of the normal audio buffering amount

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

FILE* xyz;

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

  m_bufferDuration = BUF_CHUNKS * m_bufOpRange; // Decide how long (in milliseconds) the DSound buffer should be
  m_deviceName = DSoundGetName(&m_deviceGUID);  // Obtain information about the device (name and GUID)

  // Create the garbage-collector thread (manages packets that have finished playing)
  m_gcThread.Create(this, true);      /* TODO: check that creation was successful */
  m_gcThread.SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
  m_gcThread.Resume();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

xyz=fopen("buffer.txt","at");
  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  // Release Directsound
  if (m_lpDirectSound != NULL)
    DSoundClose();

  // Signal the GC thread to quit
  if (m_gcThread.GetThreadHandle() != NULL)
    m_gcThread.Cancel();

if(xyz) fclose(xyz);
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
  HRESULT hrThis = S_OK, hrThat = S_OK, hr;

  if ((m_lpDirectSound == NULL) && (!DSoundOpen(false))) {
    hrThis = S_FALSE;         // The device is not open, and an attempt to open it failed
  } else {
    _ASSERTE(m_lpDirectSound != NULL);

    // Check if the buffer has to be (re)created either because it was not
    //  created before, or following a change in playback format that requires
    //  the buffer to be released then created again with the new format
    if ((m_lpDirectSoundBuffer == NULL) ||
        (m_waveFormat.nChannels != channels) ||
        (m_waveFormat.nSamplesPerSec * (1.00 - TOLERANCE_FREQ / 100.0) > samplesPerSec) || // accept 10% tolerance in freq. change, avoids excessive overhead in
        (m_waveFormat.nSamplesPerSec * (1.00 + TOLERANCE_FREQ / 100.0) < samplesPerSec) || // closing/re-opening the device (some games abuse the time-constant)
        (m_waveFormat.wBitsPerSample != bitsPerSample))
    {
      // Gain exclusive access to the DSound buffer and related variables
      CSingleLock lock(&m_mutex, TRUE);

      // If the buffer is created, try to release it first
      if (m_lpDirectSoundBuffer != NULL) {
        m_lpDirectSoundBuffer->Stop();
        m_lpDirectSoundBuffer->Release();
        m_lpDirectSoundBuffer = NULL;
      }

      // Change the format
      m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
      m_waveFormat.nChannels = channels;
      m_waveFormat.nSamplesPerSec = samplesPerSec;
      m_waveFormat.nAvgBytesPerSec = channels * samplesPerSec * bitsPerSample / 8;
      m_waveFormat.nBlockAlign = channels * bitsPerSample / 8;
      m_waveFormat.wBitsPerSample = bitsPerSample;
      m_waveFormat.cbSize = 0;

      // Compute desired buffer length (in bytes), then make sure
      //  the value falls within the acceptable DirectSound bounds
      m_bufferLen = (int)((m_bufferDuration / 1000.0) * m_waveFormat.nAvgBytesPerSec);
      m_bufferLen = max(DSBSIZE_MIN, min(DSBSIZE_MAX, m_bufferLen));

      // Fill in the buffer-creation structure
      DSBUFFERDESC DSBufferDesc;
      memset(&DSBufferDesc, 0, sizeof(DSBufferDesc));
      DSBufferDesc.dwSize = sizeof(DSBufferDesc);
      DSBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
      DSBufferDesc.dwBufferBytes = m_bufferLen;
      DSBufferDesc.dwReserved = 0;
      DSBufferDesc.lpwfxFormat = &m_waveFormat;

      // Create the buffer with the new format
      if (FAILED(hr = m_lpDirectSound->CreateSoundBuffer(&DSBufferDesc, &m_lpDirectSoundBuffer, NULL)))
        hrThis = hr;

      // Initialize local buffer state
      m_bufferPos = 0;      // we are at the start of the buffer (no data is enqueued yet)
      m_DSoundLatency = 0;  // the DSound latency for this audio format is unknown yet
      m_playedBytes = 0;    // no bytes went through DSound yet
      m_sentBytes = 0;      // no bytes came from the application for playback yet

      // Compute the ideal amount of buffering (in bytes)
      m_bufferedLo = m_waveFormat.nAvgBytesPerSec * m_bufOpRange / 1000;
      m_bufferedHi = m_bufferedLo * 2;
    }
  }

  // Forward the call to other module(s) daisy-chained after us (if any)
  if (m_waveOut != NULL)
    hrThat = m_waveOut->SetFormat(channels, samplesPerSec, bitsPerSample);

  return (hrThat == S_OK ? hrThis : hrThat);
}

STDMETHODIMP CWaveOut::PlayData(BYTE * data, LONG length, DOUBLE * load) {
  if (data == NULL)
    return E_POINTER;

  if (load == NULL)
    return E_POINTER;

  HRESULT hrThis = S_OK, hrThat = S_OK, hr;
  DOUBLE loadThis = 1.0, loadThat = 1.0;

  if ((m_lpDirectSound == NULL) && (!DSoundOpen(false))) {
    hrThis = S_FALSE;         // The device is not open, and an attempt to open it failed
  } else {
    _ASSERTE(m_lpDirectSound != NULL);

    try {
      // Gain exclusive access to the DSound buffer and related variables
      CSingleLock lock(&m_mutex, TRUE);

      // Verify whether the DirectSound buffer exists
      if (m_lpDirectSoundBuffer == NULL)
        throw E_FAIL;

      // Write the new audio data to the DirectSound buffer
      BYTE* data1, * data2;
      DWORD length1, length2;
      DWORD maxLength = min(length, m_bufferLen);

      if (FAILED(hr = m_lpDirectSoundBuffer->Lock(m_bufferPos, maxLength, (LPVOID*)(&data1), &length1, (LPVOID*)(&data2), &length2, 0)))
        throw hr;

      if (data1 != NULL) memcpy(data1, data, length1);
      if (data2 != NULL) memcpy(data2, data + length1, length2);

      m_lpDirectSoundBuffer->Unlock(data1, length1, data2, length2);

      // Make sure the buffer isn't stopped
      if (FAILED(hr = m_lpDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING)))
        hrThis = hr;

      // Compute by how much we are leading the head-cursor of buffer playback 
      DWORD dwCurrentWriteCursor = 0, dwCurrentReadCursor = 0;
      LONG  bufferedBytes;

      if (FAILED(hr = m_lpDirectSoundBuffer->GetCurrentPosition(&dwCurrentReadCursor, &dwCurrentWriteCursor)))
        hrThis = hr;

      m_DSoundLatency = max(m_DSoundLatency, (LONG)(m_bufferLen + dwCurrentWriteCursor - dwCurrentReadCursor) % m_bufferLen);
      bufferedBytes = (2l * m_bufferLen + m_bufferPos - dwCurrentReadCursor - m_DSoundLatency) % m_bufferLen;

      if (bufferedBytes > (m_bufferLen - m_bufferedLo))
        bufferedBytes = 0;      // Take into account the possibility that we were left behind

//if (xyz) fprintf(xyz,"%6d / %6d (%6d %6d %6d)\n",(int)bufferedBytes,(int)m_bufferLen,(int)dwCurrentReadCursor,(int)dwCurrentWriteCursor,(int)m_bufferPos);

      // Advance our write cursor, and count the data as successfully "sent"
      m_bufferPos = (m_bufferPos + maxLength) % m_bufferLen;
      m_sentBytes += maxLength;

      // Compute how off-target we are with buffering
      if (bufferedBytes < m_bufferedLo) {
        loadThis = (double)bufferedBytes / m_bufferedLo;
      } else if (bufferedBytes > m_bufferedHi) {
        loadThis = (double)bufferedBytes / m_bufferedHi;
      }
    } catch (HRESULT hr) {
      hrThis = hr;              // Propagate the error
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T(">> ERROR PlayData: 0x%08x - %s"), (int)hr, (LPCTSTR)FormatMessage(hr)));
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
  DWORD dwLastReadCursor = 0;

  _ASSERTE(thread.GetThreadID() == m_gcThread.GetThreadID());

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread created (handle = 0x%08x, ID = %d)"), (int)thread.GetThreadHandle(), (int)thread.GetThreadID()));

  while (true) {
    if (thread.GetMessage(&message, false)) {       // non-blocking message-"peek"
      switch (message.message) {
        case WM_QUIT:
          RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Garbage collector thread cancelled")));
          return 0;

        default:
          break;
      }
    } else {
      try {
        // Gain exclusive access to the DSound buffer and related variables
        CSingleLock lock(&m_mutex, TRUE);

        if (m_lpDirectSoundBuffer != NULL) {
          HRESULT hr;

          // First find out where in the buffer we are playing back from
          DWORD dwCurrentReadCursor = 0;

          if (FAILED(hr = m_lpDirectSoundBuffer->GetCurrentPosition(&dwCurrentReadCursor, NULL)))
            throw hr;

          // Compute how much bytes were played back since we last checked,
          //  and add the amount to the "played bytes" grand total
          if (m_playedBytes == 0) {
            m_playedBytes = dwCurrentReadCursor;  // the buffer is brand-new
          } else {
            m_playedBytes += (m_bufferLen + dwCurrentReadCursor - dwLastReadCursor) % m_bufferLen;
          }

          dwLastReadCursor = dwCurrentReadCursor; // remember this for the next check to find the "played" increment

          _ASSERTE(m_playedBytes % m_bufferLen == (LONG)dwCurrentReadCursor);
          _ASSERTE(m_sentBytes % m_bufferLen == m_bufferPos);

          // Compute what portion of the buffer has not been replenished and needs to be silenced
          LONG dirtyBytes = m_playedBytes - m_sentBytes;

          if (dirtyBytes > 0) {                   // if lagging, flush the buffer
            m_sentBytes += (m_bufferLen + dirtyBytes - (dirtyBytes % m_bufferLen)); // get rid of the huge lag, or else the condition may trigger indefinitely
            dirtyBytes = m_bufferLen;             // the buffer has been starved for too long, all data is direty and must be silenced
          } else if (dirtyBytes < -m_bufferLen) { // freak case; either playback stalled, or this thread was delayed and lost playback updates
            dirtyBytes = -dirtyBytes;             // take the absoulute value
            m_playedBytes += (dirtyBytes - (dirtyBytes % m_bufferLen)); // get rid of the huge lag, or else the condition may trigger several times
            dirtyBytes %= m_bufferLen;            // preventively erase data between the write and (adjusted) play position
          } else {                                // in all other cases, preventively erase data between the write and play cursors
            dirtyBytes = (m_bufferLen + dirtyBytes) % m_bufferLen;
          }

          if (dirtyBytes > 0) {
            // Write silence data to the DirectSound buffer
            BYTE* data1, * data2;
            DWORD length1, length2;
            int silenceData = ((m_waveFormat.wBitsPerSample == 8) ? 0x80 : 0x00);

            if (FAILED(hr = m_lpDirectSoundBuffer->Lock(m_bufferPos, dirtyBytes, (LPVOID*)(&data1), &length1, (LPVOID*)(&data2), &length2, 0)))
              throw hr;

            if (data1 != NULL) memset(data1, silenceData, length1);
            if (data2 != NULL) memset(data2, silenceData, length2);

            if (FAILED(hr = m_lpDirectSoundBuffer->Unlock(data1, length1, data2, length2)))
              throw hr;
          }

if(xyz)fprintf(xyz,"> %6d %6d < [ %6d / %6d ] %9d\n",(int)m_bufferPos,(int)dwCurrentReadCursor,(int)dirtyBytes,(int)m_bufferLen,(int)(m_playedBytes - m_sentBytes));
        }
      } catch (HRESULT hr) {
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T(">> ERROR Run: 0x%08x - %s"), (int)hr, (LPCTSTR)FormatMessage(hr)));
      }

      Sleep(m_bufferDuration / 2);
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
// Callback function invoked by DirectSoundEnumerate
//
BOOL CALLBACK CWaveOut::DSEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext) {
  DSDeviceInfoArray* lpInfoArray = (DSDeviceInfoArray*)lpContext;

  try {
    if (lpInfoArray != NULL) {
      // The results array will contain at least one entry; the first entry
      //  must always be the default DSound device (with a NULL GUID)
      if (lpInfoArray->GetSize() == 0) {
        lpInfoArray->Add(DSDeviceInfo(GUID_NULL, _T(""), _T("")));
      }

      if (lpGuid == NULL) {
        // This is the default device (NULL GUID), just fill in the details
        //  (the description and module name are of interest)
        lpInfoArray->SetAt(0, DSDeviceInfo(GUID_NULL, lpcstrDescription, lpcstrModule));
      } else {
        // This is a specific DirectSound device, fill in the details (GUID,
        //  description and module name are all needed)
        lpInfoArray->Add(DSDeviceInfo(*lpGuid, lpcstrDescription, lpcstrModule));
      }
    }
  } catch (CMemoryException * /*cme*/) { }

  return TRUE;
}

//
// Opens the DSound device specified in the module's settings
//
bool CWaveOut::DSoundOpen(bool isInteractive) {
  static bool isErrPrompt = true;
  static bool isErrLog    = true;
  static time_t lastRetry = 0;

  // Don't open the device if it's already open
  if (m_lpDirectSound != NULL)
    return true;

  // Only attempt to open the device at reasonable intervals, to avoid excessive overhead
  if ((time(NULL) - lastRetry) < WAVEOPEN_RETRY_INTERVAL) {
    return false;           // another attempt to open the device was made very recently; don't overdo it
  } else {
    lastRetry = time(NULL);
  }

  HRESULT hr;

  // Attempt to open the DSound device
  while (FAILED(hr = DSoundOpenHelper())) {
    if (isErrLog) {         // do we log the error ?
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Could not open the device %d ('%s'):\n0x%08x - %s"), m_deviceID, (LPCTSTR)m_deviceName, (int)hr, (LPCTSTR)FormatMessage(hr)));
      isErrLog = false;     // only log this error once, as it will probably (re)occur several times in a row
    }

    if (!isInteractive)     // do we want to pop up a message box ?
      break;                // stop trying

    if (!isErrPrompt)       // did the user select not to be prompted again ?
      break;                // stop trying

    if (MessageBox(FormatMessage(MSG_ERR_E_OPENDEVICE, false, m_deviceID, (LPCTSTR)m_deviceName, (int)hr, (LPCTSTR)FormatMessage(hr)),
                   _T("DirectSound Device Error"), /* TODO: LoadString(...) */
                   MB_RETRYCANCEL, MB_ICONERROR) == IDCANCEL)
    {
      isErrPrompt = false;  // the user does not whish to be prompted again !
      break;                // stop trying
    }
  }

  if (m_lpDirectSound != NULL) {  // The device was opened successfully
    isErrLog    = true;     // Next time we get an error, log it
    isErrPrompt = true;     // Next time we get an error, prompt the user (if interactive)
    return true;
  } else {                  // The device could not be open, and the user did not retry
    return false;
  }
}

//
// Closes the DSound device specified in the module's settings
//
void CWaveOut::DSoundClose(void) {
  if (m_lpDirectSoundBuffer != NULL) {
    m_lpDirectSoundBuffer->Release(); // Release the COM interface
    m_lpDirectSoundBuffer = NULL;     // Lose the pointer
  }

  if (m_lpDirectSound != NULL) {
    m_lpDirectSound->Release();       // Release the COM interface
    m_lpDirectSound = NULL;           // Lose the pointer
  }
}

//
//
//
HRESULT CWaveOut::DSoundOpenHelper(void) {
  // Don't initialize more than once
  if (m_lpDirectSound != NULL)
    return S_OK;

  HRESULT hr;
  LPDIRECTSOUND lpDirectSound;

  try {
    // Initialize DirectSound
    if (FAILED(hr = DirectSoundCreate(m_deviceGUID == GUID_NULL ? NULL : &m_deviceGUID, &lpDirectSound, NULL)))
      throw hr;

    _ASSERTE (lpDirectSound != NULL);

    // Must set the coopertaive level before working with DSound
    if (FAILED(hr = lpDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)))
      throw hr;

    // Obtain the device capabilities
    DSCAPS DSCaps;
    memset(&DSCaps, 0, sizeof(DSCaps));
    DSCaps.dwSize = sizeof(DSCaps);

    if (SUCCEEDED(hr = lpDirectSound->GetCaps(&DSCaps))) {
      int channels      = (DSCaps.dwFlags & DSCAPS_PRIMARYSTEREO) ? 2 : 1;
      int bitsPerSample = (DSCaps.dwFlags & DSCAPS_PRIMARY16BIT) ? 16 : 8;
      int samplesPerSec = 8000;

      // Obtain the primary buffer
      LPDIRECTSOUNDBUFFER lpPrimary = NULL;

      DSBUFFERDESC DSBufferDesc;
      memset(&DSBufferDesc, 0, sizeof(DSBufferDesc));
      DSBufferDesc.dwSize = sizeof(DSBufferDesc);
      DSBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
      DSBufferDesc.dwBufferBytes = 0;
      DSBufferDesc.dwReserved = 0;
      DSBufferDesc.lpwfxFormat = NULL;  // must be NULL for primary buffers

      if (SUCCEEDED(hr = lpDirectSound->CreateSoundBuffer(&DSBufferDesc, &lpPrimary, NULL)) && (lpPrimary != NULL)) {
        // Change the format
        WAVEFORMATEX waveFormat;

        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = channels;
        waveFormat.nSamplesPerSec = samplesPerSec;
        waveFormat.nAvgBytesPerSec = channels * samplesPerSec * bitsPerSample / 8;
        waveFormat.nBlockAlign = channels * bitsPerSample / 8;
        waveFormat.wBitsPerSample = bitsPerSample;
        waveFormat.cbSize = 0;

        if (SUCCEEDED(hr = lpPrimary->SetFormat(&waveFormat))) {
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, _T("WOW!!!!"));
        } else {
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("FuBiGa: SetFormat: %08x"), hr));
        }
      } else {
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("FkBlGs: CreateSoundBuffer: %08x"), hr));
      }
    } else {
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("FcBGat: GetCaps: %08x"), hr));
    }
  } catch (HRESULT hr) {
    if (lpDirectSound != NULL)
      lpDirectSound->Release(); // Release the (improperly initialized) interface

    return hr;                  // Propagate the error
  }

  m_lpDirectSound = lpDirectSound;
  return S_OK;
}

//
//
//
CString CWaveOut::DSoundGetName(LPGUID lpGUID) {
  // Obtain device information for the given device ID
  int deviceIdx = m_deviceID + 1;

  DSDeviceInfoArray infoArray;
  DirectSoundEnumerate(DSEnumCallback, &infoArray);

  if ((deviceIdx >= 0) && (deviceIdx < infoArray.GetSize())) {
    if (lpGUID) *lpGUID = infoArray[deviceIdx].guid;
    return infoArray[deviceIdx].description + _T(" / ") + infoArray[deviceIdx].module;
  } else {
    RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Invalid DirectSound device ID (%d), value must lie between -1 and %d"), m_deviceID, infoArray.GetSize() - 2));
    if (lpGUID) *lpGUID = GUID_NULL;
    return CString(_T("<unknown>"));
  }
}
