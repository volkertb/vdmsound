// MIDIMapper.cpp : Implementation of CMIDIMapper
#include "stdafx.h"
#include "MIDIToolkit.h"
#include "MIDIMapper.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_IO_OPEN         _T("An error was encountered while attempting to open file '%1' for reading.%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")
#define MSG_ERR_IO              _T("Unknown I/O error encountered while accessing the file '%1':%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")
#define MSG_ERR_PARSE_SYNTAX    _T("Syntax error (%1):%nExpected '%3', but found '%2' instead.%0")
#define MSG_ERR_PARSE_FORMAT    _T("Unexpected data (%1):%nA non-empty line was encountered before the start of a section.%0")
#define MSG_ERR_PARSE_CONFLICT  _T("Symbol redefinition (%1):%nThe entry '%3' was already defined (see %2).%0")
#define MSG_ERR_PARSE           _T("Unknown parsing error (%1):%nDescription not available.%0")

#define MSG_ERR_MAP_SECTION     _T("Could not find the INI section [%2] in file '%1'.%0")
#define MSG_ERR_MAP_UNEXPECTED  _T("An unexpected condition was encountered while trying to access key '%3' in section [%2] of file '%1'.%0")
#define MSG_ERR_MAP_NUMERICAL   _T("The value '%2' (see %1) is not numerical.  Please provide a numerical value.%0")
#define MSG_ERR_MAP_RANGE       _T("The value '%2!d!' (see %1) is out of range.  Please provide a value between %3!d! and %4!d!.%0")

/////////////////////////////////////////////////////////////////////////////

#define GET_EVENT(data)           ((data) & 0xf0)
#define MIDI_EVENT_PROGRAM_CHANGE 0xc0

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_MAPFNAME      L"MapFile"
#define INI_STR_MIDIOUT       L"MidiOut"

#define INI_STR_PATCHMAP      "patches"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

#include <INIParser.h>
#pragma comment ( lib , "INIParser.lib" )

/////////////////////////////////////////////////////////////////////////////
// CMIDIMapper

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIMapper::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CMIDIMapper::Init(IUnknown * configuration) {
	if (configuration == NULL)
		return E_POINTER;

  HRESULT hr;

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Obtain the Query objects (for intialization purposes)
  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends(configuration);  // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config(configuration);  // Configuration query object

  try {
    // Obtain the name of the file containing the mappings
    _bstr_t mapFileName = Config->Get(INI_STR_MAPFNAME);

    if (FAILED(hr = loadMapping(mapFileName)))
      return hr;

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, false);   // complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIMapper initialized")));

  return S_OK;
}

STDMETHODIMP CMIDIMapper::Destroy() {
  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIMapper released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIMapper::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  _ASSERTE(data1 < 128);

  if (GET_EVENT(status) == MIDI_EVENT_PROGRAM_CHANGE)
    data1 = m_patchMap[data1];      // Program change

  if (m_midiOut != NULL)
    return m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return S_OK;
}

STDMETHODIMP CMIDIMapper::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
	if (data == NULL)
		return E_POINTER;

  if (m_midiOut != NULL)
    return m_midiOut->HandleSysEx(usDelta, data, length);

  return S_OK;
}

STDMETHODIMP CMIDIMapper::HandleRealTime(LONGLONG usDelta, BYTE data) {
  if (m_midiOut != NULL)
    return m_midiOut->HandleRealTime(usDelta, data);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

HRESULT CMIDIMapper::loadMapping(LPCSTR fName) {
  // Reset the mapping to identity
  for (int i = 0; i < 128; i++) {
    m_patchMap[i] = i;
  }

  // Open the .INI file containig the mapping information
  CINIParser map;

  try {
    map.load(fName);
  } catch (CINIParser::fopen_error& foe) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_IO_OPEN, /*false, NULL, 0, */false, (LPCTSTR)CString(foe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::io_error& ioe) {
    DWORD lastError = GetLastError();
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_IO, /*false, NULL, 0, */false, (LPCTSTR)CString(ioe.location.c_str()), lastError, (LPCTSTR)FormatMessage(lastError)), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::syntax_error& se) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_SYNTAX, /*false, NULL, 0, */false, (LPCTSTR)CString(se.location.c_str()), (LPCTSTR)CString(se.offender.c_str()), (LPCTSTR)CString(se.expected.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::format_error& fe) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_FORMAT, /*false, NULL, 0, */false, (LPCTSTR)CString(fe.location.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::conflict_error& ce) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE_CONFLICT, /*false, NULL, 0, */false, (LPCTSTR)CString(ce.location.c_str()), (LPCTSTR)CString(ce.defLocation.c_str()), (LPCTSTR)CString(ce.redefined.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  } catch (CINIParser::parse_error& pe) {
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_PARSE, /*false, NULL, 0, */false, (LPCTSTR)CString(pe.location.c_str())), __uuidof(IVDMBasicModule), E_ABORT);
  }

  // Get the patch mapping
  striset_t patchMapKeys;

  if (!map.getKeys(INI_STR_PATCHMAP, patchMapKeys))
    return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_SECTION, /*false, NULL, 0, */false, (LPCTSTR)CString(fName), (LPCTSTR)CString(INI_STR_PATCHMAP)), __uuidof(IVDMBasicModule), E_ABORT);

  // Retrieve the re-mapped patches and store them locally
  for (striset_t::const_iterator itKey = patchMapKeys.begin(); itKey != patchMapKeys.end(); itKey++) {
    int mapFrom, mapTo;
    std::string value, keyLoc;

    if (!map.getLocation(INI_STR_PATCHMAP, *itKey, keyLoc))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_UNEXPECTED, /*false, NULL, 0, */false, (LPCTSTR)CString(fName), (LPCTSTR)CString(INI_STR_PATCHMAP), (LPCTSTR)CString(itKey->c_str())), __uuidof(IVDMBasicModule), E_UNEXPECTED);
    if (!map.getValue(INI_STR_PATCHMAP, *itKey, value))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_UNEXPECTED, /*false, NULL, 0, */false, (LPCTSTR)CString(fName), (LPCTSTR)CString(INI_STR_PATCHMAP), (LPCTSTR)CString(itKey->c_str())), __uuidof(IVDMBasicModule), E_UNEXPECTED);

    if (sscanf(itKey->c_str(), "%d", &mapFrom) != 1)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_NUMERICAL, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(itKey->c_str())), __uuidof(IVDMBasicModule), E_ABORT);
    if (sscanf(value.c_str(), "%d", &mapTo) != 1)
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_NUMERICAL, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), (LPCTSTR)CString(value.c_str())), __uuidof(IVDMBasicModule), E_ABORT);

    if ((mapFrom < 0) || (mapFrom > 127))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), mapFrom, 0, 127), __uuidof(IVDMBasicModule), E_ABORT);
    if ((mapTo < 0) || (mapTo > 127))
      return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_MAP_RANGE, /*false, NULL, 0, */false, (LPCTSTR)CString(keyLoc.c_str()), mapTo, 0, 127), __uuidof(IVDMBasicModule), E_ABORT);

    m_patchMap[mapFrom] = mapTo;
  }

  return S_OK;
}
