// DeviceUtil.h: interface for the DeviceUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEUTIL_H__3B79B7EA_56DF_41A8_9089_E7FFEF7CF16F__INCLUDED_)
#define AFX_DEVICEUTIL_H__3B79B7EA_56DF_41A8_9089_E7FFEF7CF16F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// DeviceUtil
//////////////////////////////////////////////////////////////////////

namespace DeviceUtil {
  // Types
  enum DeviceType { DEV_UNKNOWN = -1, DEV_MIDI = 0, DEV_WAVE = 1, DEV_DSOUND = 2 };

  struct DeviceInfo {
    DeviceInfo(void) : deviceType(DEV_UNKNOWN), deviceID(-1), deviceName(_T(""))
      { }
    DeviceInfo(const DeviceInfo& src) : deviceType(src.deviceType), deviceID(src.deviceID), deviceName(src.deviceName)
      { }
    DeviceInfo& operator=(const DeviceInfo& rhs)
      { deviceType = rhs.deviceType; deviceID = rhs.deviceID; deviceName = rhs.deviceName; return *this; }

    DeviceType deviceType;
    UINT deviceID;
    CString deviceName;
  };

  typedef CArray<DeviceInfo,const DeviceInfo&> DeviceInfoList;

  // Methods
  LPCTSTR GetDevTypeText(DeviceType type);

  HRESULT EnumMidiOut(DeviceInfoList& result);
  HRESULT EnumWaveOut(DeviceInfoList& result);
  HRESULT EnumDSoundOut(DeviceInfoList& result);
};

#endif // !defined(AFX_DEVICEUTIL_H__3B79B7EA_56DF_41A8_9089_E7FFEF7CF16F__INCLUDED_)
