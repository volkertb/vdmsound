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

int _strmcmpi(const char* templ, ... );

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

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration and VDM services
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends = configuration;    // Dependency query object
    Config  = configuration;    // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain indicator-led settings, use defaults if none specified
    _bstr_t ledName = CFG_Get(Config, INI_STR_LEDID, "SCROLL", false);
    switch (_strmcmpi((LPCSTR)ledName, "NUM", "CAPS", "SCROLL", "none", NULL)) {
      case 0:
        m_ledID = LED_NUMLOCK;
        break;
      case 1:
        m_ledID = LED_CAPSLOCK;
        break;
      case 2:
        m_ledID = LED_SCROLLLOCK;
        break;
      case 3:
        m_ledID = LED_NONE;
        break;
      default:
        m_ledID = LED_NONE;
        RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("An invalid value ('%s') was provided for the led identifier ('%s').  Valid values are: 'NUM', 'CAPS', 'SCROLL' and 'none'.\nUsing 'none' by default."), (LPCTSTR)ledName, (LPCTSTR)CString(INI_STR_LEDID)));
    }

    /** Get modules ********************************************************/

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, true);   // do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("ActivityLights initialized (led = %s)"), m_ledID == LED_NUMLOCK ? _T("NUM") : m_ledID == LED_CAPSLOCK ? _T("CAPS") : m_ledID == LED_SCROLLLOCK ? _T("SCROLL") : m_ledID == LED_NONE ? _T("(none)") : _T("???")));

  return S_OK;
}

STDMETHODIMP CActivityLights::Destroy() {
  // Reset the lights
  if ((m_ledID != -1) && m_isIndicatorOn) {
    m_isIndicatorOn = false;
    SetLedStatus(m_ledID, !GetLedStatus(m_ledID));
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
  if ((m_ledID != -1) && m_isIndicatorOn) {
    m_isIndicatorOn = false;
    SetLedStatus(m_ledID, !GetLedStatus(m_ledID));
  }

  if (m_midiOut != NULL)
    return m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return S_OK;
}

STDMETHODIMP CActivityLights::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
	if (data == NULL)
		return E_POINTER;

  if (m_ledID != -1) {
    m_isIndicatorOn = !m_isIndicatorOn;
    SetLedStatus(m_ledID, !GetLedStatus(m_ledID));
  }

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

int _strmcmpi(
    const char* templ,              // the string to compare
    ... )                           // list of strings to compare agains; NULL means end of list
{
  va_list args;
  va_start(args, templ);            // Initialize variable arguments

  const char* against;
  int count = 0, match = -1;

  // Go through supplied arguments until a match is found or list ends
  while ((against = va_arg(args, const char*)) != NULL) {
    if (_strcmpi(templ, against) == 0) {
      match = count;
      break;
    } else {
      count++;
    }
  }

  va_end(args);                     // Reset variable arguments

  return match;
}
