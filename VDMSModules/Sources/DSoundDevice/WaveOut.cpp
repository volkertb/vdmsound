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

  m_deviceName = DSoundGetName(&m_deviceGUID);  // Obtain information about the device (name and GUID)

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

xyz=fopen("buffer.txt","at");
  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  // Release Directsound
  if (m_lpDirectSound != NULL)
    DSoundClose();

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
      m_bufferLen = (int)(BUF_CHUNKS * (m_bufOpRange / 1000.0) * m_waveFormat.nAvgBytesPerSec);
      m_bufferLen = max(DSBSIZE_MIN, min(DSBSIZE_MAX, m_bufferLen));

      // Fill in the buffer-creation structure
      DSBUFFERDESC DSBufferDesc;
      DSBufferDesc.dwSize = sizeof(DSBufferDesc);
      DSBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
      DSBufferDesc.dwBufferBytes = m_bufferLen;
      DSBufferDesc.dwReserved = 0;
      DSBufferDesc.lpwfxFormat = &m_waveFormat;

      // Create the buffer with the new format
      if (FAILED(hr = m_lpDirectSound->CreateSoundBuffer(&DSBufferDesc, &m_lpDirectSoundBuffer, NULL)))
        hrThis = hr;

      // No bytes are enqueued for playback (yet)
      m_bufferPos = 0;
      m_DSoundLatency = 0;

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
      // Verify whether the DirectSound buffer exists
      if (m_lpDirectSoundBuffer == NULL)
        throw _com_error(E_FAIL);

      // Write the new audio data to the DirectSound buffer
      BYTE* data1, * data2;
      DWORD length1, length2;

      if (FAILED(hr = m_lpDirectSoundBuffer->Lock(m_bufferPos, min(length, m_bufferLen), (LPVOID*)(&data1), &length1, (LPVOID*)(&data2), &length2, 0)))
        throw _com_error(hr);

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

      // Advance our write cursor
      m_bufferPos = (m_bufferPos + length) % m_bufferLen;

      // Compute how off-target we are with buffering
      if (bufferedBytes < m_bufferedLo) {
        loadThis = (double)bufferedBytes / m_bufferedLo;
      } else if (bufferedBytes > m_bufferedHi) {
        loadThis = (double)bufferedBytes / m_bufferedHi;
      }
    } catch (_com_error& ce) {
      hrThis = ce.Error();      // Propagate the error
RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T(">> ERROR: 0x%08x - %s"), (int)hrThis, (LPCTSTR)FormatMessage(hrThis)));
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
  if (m_lpDirectSound != NULL) {
    m_lpDirectSound->Release();
    m_lpDirectSound = NULL;   // Lose the pointer
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
      throw _com_error(hr);

    _ASSERTE (lpDirectSound != NULL);

    // Must set the coopertaive level before working with DSound
    if (FAILED(hr = lpDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)))
      throw _com_error(hr);

    /* TODO: set the pirmary buffer format to 44100Hz 16bit stereo */
  } catch (_com_error& ce) {
    if (lpDirectSound != NULL)
      lpDirectSound->Release(); // Release the (improperly initialized) interface

    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();          // Propagate the error
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
