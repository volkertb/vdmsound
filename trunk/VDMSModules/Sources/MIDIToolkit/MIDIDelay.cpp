// MIDIDelay.cpp : Implementation of CMIDIDelay
#include "stdafx.h"
#include "MIDIToolkit.h"
#include "MIDIDelay.h"

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_MIDIOUT       L"MidiOut"

#define INI_STR_DELAY         "delay"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CMIDIDelay

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIDelay::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CMIDIDelay::Init(IUnknown * configuration) {
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

    // Obtain the name of the file containing the mappings
    m_delay = CFG_Get(Config, INI_STR_DELAY, 0, 10, false);

    /** Get modules ********************************************************/

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, false);   // complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIDelay initialized")));

  return S_OK;
}

STDMETHODIMP CMIDIDelay::Destroy() {
  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIDelay released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIDelay::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  if (m_delay > 0)
    Sleep(m_delay);

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("Slept for %dms"), m_delay));

  if (m_midiOut != NULL)
    return m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return S_OK;
}

STDMETHODIMP CMIDIDelay::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
	if (data == NULL)
		return E_POINTER;

  if (m_midiOut != NULL)
    return m_midiOut->HandleSysEx(usDelta, data, length);

  return S_OK;
}

STDMETHODIMP CMIDIDelay::HandleRealTime(LONGLONG usDelta, BYTE data) {
  if (m_midiOut != NULL)
    return m_midiOut->HandleRealTime(usDelta, data);

  return S_OK;
}
