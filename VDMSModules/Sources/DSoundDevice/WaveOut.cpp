// WaveOut.cpp : Implementation of CWaveOut
#include "stdafx.h"
#include "DSoundDevice.h"
#include "WaveOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in .mc file or String Table resource */

#define MSG_ERR_DEVICEID      _T("Invalid DirectSound device ID (%1!d!), value must lie between -1 and %2!d!.")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_DEVICEID      L"device"
#define INI_STR_BUFOPRANGE    L"buffer"
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
  HRESULT hr;

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

  // Initialize DirectSound
  if (FAILED(hr = DSoundInit()))
    return hr;

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized (device ID = %d, '%s')"), m_deviceID, (LPCTSTR)m_deviceName));

  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  // Release Directsound
  if (m_lpDirectSound != NULL)
    m_lpDirectSound->Release();

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
  HRESULT hrThis = S_OK, hrThat = S_OK;

  /* TODO: implement */

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

  /* TODO: implement */

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
  } catch (CMemoryException& /*cme*/) { }

  return TRUE;
}

//
//
//
HRESULT CWaveOut::DSoundInit(void) {
  HRESULT hr;

  // Obtain device information for the given device ID
  int deviceIdx = m_deviceID + 1;

  DSDeviceInfoArray infoArray;
  DirectSoundEnumerate(DSEnumCallback, &infoArray);

  if ((deviceIdx >= 0) && (deviceIdx < infoArray.GetSize())) {
    m_deviceGUID = infoArray[deviceIdx].guid;
    m_deviceName = infoArray[deviceIdx].description + _T(" / ") + infoArray[deviceIdx].module;
  } else {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_DEVICEID, /*false, NULL, 0, */false, m_deviceID, infoArray.GetSize() - 2), __uuidof(IVDMBasicModule), E_INVALIDARG);
  }

  // Initialize DirectSound
  if (FAILED(hr = DirectSoundCreate(deviceIdx == 0 ? NULL : &m_deviceGUID, &m_lpDirectSound, NULL)))
    return hr;

  _ASSERTE (m_lpDirectSound != NULL);

  if (FAILED(hr = m_lpDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)))
    return hr;

  return S_OK;
}
