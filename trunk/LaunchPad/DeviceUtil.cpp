// DeviceUtil.cpp: implementation of the DeviceUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "launchpad.h"
#include "DeviceUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct DSEnumCallback_struct {
  DSEnumCallback_struct(DeviceUtil::DeviceInfoList& _devInfo) : lastDevID(0), devInfo(_devInfo)
    { }
  int lastDevID;
  DeviceUtil::DeviceInfoList& devInfo;
};

// DSound enumerator callback fn.
BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext) {
  DSEnumCallback_struct* pDevInfo = (DSEnumCallback_struct*)lpContext;

  ASSERT(pDevInfo != NULL);

  if (pDevInfo != NULL) {
    DeviceUtil::DeviceInfo info;

    info.deviceType = DeviceUtil::DEV_DSOUND;
    info.deviceName = lpcstrDescription;

    if (lpGuid == NULL) {
      info.deviceID = -1;
    } else {
      info.deviceID = pDevInfo->lastDevID++;
    }

    pDevInfo->devInfo.Add(info);
  }

  return TRUE;
}

//
//
//
LPCTSTR DeviceUtil::GetDevTypeText(DeviceType type) {
  switch (type) {
    case DEV_MIDI:
      return _T("MIDI");

    case DEV_WAVE:
      return _T("Wave");

    case DEV_DSOUND:
      return _T("DirectSound");

    case DEV_UNKNOWN:
    default:
      return _T("???");
  }
}

//
//
//
HRESULT DeviceUtil::EnumMidiOut(DeviceInfoList& result) {
  HRESULT hr   = S_OK;
  UINT numDevs = midiOutGetNumDevs();

  DeviceInfo info;
  MIDIOUTCAPS caps;

  UINT devID = MIDI_MAPPER;

  do {
    MMRESULT err = midiOutGetDevCaps(devID, &caps, sizeof(caps));

    if (err == MMSYSERR_NOERROR) {
      info.deviceType = DEV_MIDI;
      info.deviceID   = devID;
      info.deviceName = caps.szPname;
      result.Add(info);
    } else {
      hr = E_FAIL;
    }

    devID = (devID == MIDI_MAPPER ? 0 : devID + 1);
  } while (devID < numDevs);

  return hr;
}

//
//
//
HRESULT DeviceUtil::EnumWaveOut(DeviceInfoList& result) {
  HRESULT hr   = S_OK;
  UINT numDevs = waveOutGetNumDevs();

  DeviceInfo info;
  WAVEOUTCAPS caps;

  UINT devID = WAVE_MAPPER;

  do {
    MMRESULT err = waveOutGetDevCaps(devID, &caps, sizeof(caps));

    if (err == MMSYSERR_NOERROR) {
      info.deviceType = DEV_WAVE;
      info.deviceID = devID;
      info.deviceName = caps.szPname;
      result.Add(info);
    } else {
      hr = E_FAIL;
    }

    devID = (devID == WAVE_MAPPER ? 0 : devID + 1);
  } while (devID < numDevs);

  return hr;
}

//
//
//
HRESULT DeviceUtil::EnumDSoundOut(DeviceInfoList& result) {
  DSEnumCallback_struct context(result);
  return DirectSoundEnumerate(DSEnumCallback, (LPVOID)(&context));
}
