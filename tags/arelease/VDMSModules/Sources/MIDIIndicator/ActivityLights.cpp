// ActivityLights.cpp : Implementation of CActivityLights
#include "stdafx.h"
#include "MIDIIndicator.h"
#include "ActivityLights.h"

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_LEDID         L"led"
#define INI_STR_MIDIOUT       L"MidiOut"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CActivityLights

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CActivityLights::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CActivityLights::Init(IUnknown * configuration) {
	if (configuration == NULL)
		return E_POINTER;

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Obtain the Query objects (for intialization purposes)
  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends(configuration);  // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config(configuration);  // Configuration query object

  try {
    // Obtain MIDI-Out settings (if available)
    ledID = CFG_Get(Config, INI_STR_LEDID, LED_SCROLLLOCK, 10, false);

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, true);   // do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("ActivityLights initialized")));

  return S_OK;
}

STDMETHODIMP CActivityLights::Destroy() {
  // Reset the lights
  if (isIndicatorOn) {
    isIndicatorOn = false;
    SetLedStatus(ledID, !GetLedStatus(ledID));
  }

  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("ActivityLights released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CActivityLights::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  if (isIndicatorOn) {
    isIndicatorOn = false;
    SetLedStatus(ledID, !GetLedStatus(ledID));
  }

  if (m_midiOut != NULL)
    return m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return S_OK;
}

STDMETHODIMP CActivityLights::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
	if (data == NULL)
		return E_POINTER;

  isIndicatorOn = !isIndicatorOn;
  SetLedStatus(ledID, !GetLedStatus(ledID));

  if (m_midiOut != NULL)
    return m_midiOut->HandleSysEx(usDelta, data, length);

  return S_OK;
}

STDMETHODIMP CActivityLights::HandleRealTime(LONGLONG usDelta, BYTE data) {
  if (m_midiOut != NULL)
    return m_midiOut->HandleRealTime(usDelta, data);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

bool CActivityLights::__xor(bool a, bool b) {
  return ((a && (!b)) || ((!a) && b));
}

bool CActivityLights::GetLedStatus(
    int ledID)
{
  BYTE keyState[256];
  BYTE vkCode = ledID;

  GetKeyboardState((LPBYTE)(&keyState));

  return ((keyState[vkCode] & 1) != 0);
}

void CActivityLights::SetLedStatus(
    int ledID,
    bool isLedOn)
{
  BYTE vkCode = ledID;

  if (__xor(isLedOn, GetLedStatus(ledID))) {
     keybd_event( vkCode,
                  0x45,
                  KEYEVENTF_EXTENDEDKEY | 0,
                  0 );

     keybd_event( vkCode,
                  0x45,
                  KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                  0);
  }
}
