// MIDISplitter.cpp : Implementation of CMIDISplitter
#include "stdafx.h"
#include "MIDIToolkit.h"
#include "MIDISplitter.h"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////
// CMIDISplitter

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDISplitter::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CMIDISplitter::Init(IUnknown * configuration) {
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

    /* TODO: try to obtain interfaces to one or more MIDI-out modules */
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("CMIDISplitter initialized")));

  return S_OK;
}

STDMETHODIMP CMIDISplitter::Destroy() {
  /* TODO: Release the MIDI-out module */

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("CMIDISplitter released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDISplitter::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  // TODO: implement
  return E_NOTIMPL;
}

STDMETHODIMP CMIDISplitter::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
	if (data == NULL)
		return E_POINTER;

  // TODO: implement
  return E_NOTIMPL;
}

STDMETHODIMP CMIDISplitter::HandleRealTime(LONGLONG usDelta, BYTE data) {
  // TODO: implement
  return E_NOTIMPL;
}
