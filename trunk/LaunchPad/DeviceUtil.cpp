// DeviceUtil.cpp: implementation of the DeviceUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "DeviceUtil.h"

#include "LaunchPadSettings.h"
#include "LaunchpadUtil.h"

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

//
//
//
HRESULT DeviceUtil::EnumMidiIn(DeviceInfoList& result) {
  HRESULT hr   = S_OK;
  UINT numDevs = midiInGetNumDevs();

  DeviceInfo info;
  MIDIINCAPS caps;

  for (UINT devID = 0; devID < numDevs; devID++) {
    MMRESULT err = midiInGetDevCaps(devID, &caps, sizeof(caps));

    if (err == MMSYSERR_NOERROR) {
      info.deviceType = DEV_MIDI;
      info.deviceID   = devID;
      info.deviceName = caps.szPname;
      result.Add(info);
    } else {
      hr = E_FAIL;
    }
  }

  return hr;
}

//
//
//

//
// Synchronizes the state of a Win32 check-box with its corresponding
//  setting
//
HRESULT VLPUtil::SyncDevListBox(
  BOOL bSave,                                         // whether this is a set (GUI->INI) as opposed to a get (INI->GUI) operation
  CLaunchPadSettings& settings,                       // settings store
  LPCTSTR section,                                    // ini section
  const DeviceUtil::DeviceInfoList& devInfo,          // list of devices that helps translate devType/ID <-> textual representation
  CComboBox& control,                                 // combo-box control with which the data must be synchronized
  LPCTSTR devPrefix,                                  // "Out", "In", etc.
  DeviceUtil::DeviceType defDevType,                  // default device type
  LONG defDevID)                                      // default device ID
{
  ASSERT(section   != NULL);
  ASSERT(devPrefix != NULL);

  ASSERT_VALID(&control);

  HRESULT hr = S_OK;

  if (control.m_hWnd == NULL)
    return E_INVALIDARG;

  CString strDev     = CString(_T("dev"));
  CString strDevType = strDev + devPrefix + _T("Type");
  CString strDevID   = strDev + devPrefix + _T("ID");

  if (bSave) {
    int curSel = control.GetCurSel();
    DWORD selData;

    if (curSel == CB_ERR) {
      selData = CB_ERR;                               // no user data available (error)
    } else {
      selData = control.GetItemData(curSel);          // obtain the user data associated with the currently selected item
    }

    if ((selData == CB_ERR) || (selData < 1)) {       // an error was encountered, or selData denotes a "(multiple values)" (or an invalid) entry.
      settings.UnsetValue(section, strDevType);
      settings.UnsetValue(section, strDevID);
      hr = ((selData == CB_ERR) ? S_FALSE : hr);
    } else {
      HRESULT hr2;

      ASSERT((int)(selData - 1) < devInfo.GetSize());

      if (FAILED(hr2 = settings.SetValue  (section, strDevType, FormatString(_T("%d"), devInfo[selData - 1].deviceType))))
        hr = hr2;
      if (FAILED(hr2 = settings.SetValue  (section, strDevID,   FormatString(_T("%d"), devInfo[selData - 1].deviceID))))
        hr = hr2;
    }
  } else {
    BOOL bEnabled = TRUE;
    BOOL isDevTypeIndeterm, isDevIDIndeterm;
    CString devType, devID;

    HRESULT hr2;

    // If the list hasn't been populated yet then do so
    if (control.GetCount() == 0) {
      for (int i = 0; i < devInfo.GetSize(); i++) {
        int index = control.AddString(devInfo[i].deviceName + _T(" (") + DeviceUtil::GetDevTypeText(devInfo[i].deviceType) + _T(")"));

        if ((index == CB_ERR) || (index == CB_ERRSPACE) || (control.SetItemData(index, i + 1) == CB_ERR))
          hr = S_FALSE;
      }
    }

    // Index of the item that will be selected as a result of data synchronization
    int curSel = -1;

    if (FAILED(hr2 = settings.GetValue(section, strDevType, devType, &isDevTypeIndeterm, FormatString(_T("%d"), defDevType)))  ||
        FAILED(hr2 = settings.GetValue(section, strDevID,   devID,   &isDevIDIndeterm,   FormatString(_T("%d"), defDevID))))
    {
      bEnabled = FALSE;   // error encountered while retrieving data, so disable the control
      hr = hr2;
    } else if (isDevTypeIndeterm || isDevIDIndeterm) {
      // We need a "(multiple values)" item in the list because the setting is indeterminate,
      //  so insert it if it doesn't exist already
      for (int i = 0; i < control.GetCount(); i++) {  // look for existing "(multiple values)"
        if (control.GetItemData(i) == 0) {
          curSel = i;
          break;
        }
      }

      if (curSel == -1) {
        curSel = control.InsertString(0, VLPUtil::LoadString(IDS_TXT_MULTIPLEVALUES));  // add new "(multiple values)"
      }

      // Select the "(multiple values)" item (if found/successfully created)
      if ((curSel == CB_ERR) || (curSel == CB_ERRSPACE)) {
        hr = S_FALSE;
        control.SetCurSel(-1);
      } else {
        control.SetItemData(curSel, 0);
        control.SetCurSel(curSel);
      }
    } else {
      LPTSTR devTypeEndPtr = NULL, devIDEndPtr = NULL;

      long devTypeLong = _tcstol(devType, &devTypeEndPtr, 0);
      long devIDLong   = _tcstol(devID  , &devIDEndPtr,   0);

      if ((devTypeEndPtr != NULL) && (devTypeEndPtr[0] == _T('\0')) &&
          (devIDEndPtr   != NULL) && (devIDEndPtr[0]   == _T('\0')))
      {
        for (int i = 0; i < control.GetCount(); i++) {
          int selData = control.GetItemData(i);

          if ((selData != CB_ERR) && (selData > 0)) { // no error has occured and not looking at a "(multiple values)" list item
            ASSERT((int)(selData - 1) < devInfo.GetSize());

            if ((devInfo[selData - 1].deviceType == (DeviceUtil::DeviceType)devTypeLong) &&
                (devInfo[selData - 1].deviceID   == (UINT)devIDLong))
            {
              curSel = i;
              break;
            }
          } else {
            hr = ((selData == CB_ERR) ? S_FALSE : hr);
          }
        }
      }
    }

    control.SetCurSel(curSel);
    control.EnableWindow(bEnabled);
  }

  return hr;
}
