// MIDIOut.cpp : Implementation of CMIDIOut
#include "stdafx.h"
#include "DiskWriter.h"
#include "MIDIOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_IO_OPEN         _T("An error was encountered while attempting to create or to open file '%1' for writing.%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_MIDIOUT       L"MidiOut"
#define INI_STR_PATH          L"Path"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

#define SWAP_WORD(w) ((((w) >> 8) & 0x00ff) | (((w) << 8) & 0xff00))
#define SWAP_LONG(l) ((((l) >> 24) & 0x000000ff) | (((l) >> 8) & 0x0000ff00) | (((l) << 8) & 0x00ff0000) | (((l) << 24) & 0xff000000))

#define MIDI_FMT_SINGLETRACK              0x00

#define MIDI_EVENT_SYSTEM_SYSEX_MULTIPAK  0xf0  // used to send multi-packet sysex's
#define MIDI_EVENT_SYSTEM_SYSEX_ESCAPE    0xf7  // used to send sysex's as "escapes"
#define MIDI_EVENT_SYSTEM_METAEVENT       0xff
#define MIDI_EVENT_SYS_RT_RESET           0xff

#define MIDI_META_END_OF_TRACK            0x2f
#define MIDI_META_TEXT                    0x01

/////////////////////////////////////////////////////////////////////////////
// CMIDIOut

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIOut::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CMIDIOut::Init(IUnknown * configuration) {
  if (configuration == NULL)
    return E_POINTER;

  IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
  IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

  // Grab a copy of the runtime environment (useful for logging, etc.)
  RTE_Set(m_env, configuration);

  // Initialize configuration
  try {
    // Obtain the Query objects (for intialization purposes)
    Depends = configuration;    // Dependency query object
    Config  = configuration;    // Configuration query object

    /** Get settings *******************************************************/

    // Try to obtain a path where files will be stored, use current directory if none specified
    m_path  = (LPCTSTR)CFG_Get(Config, INI_STR_PATH, ".", true);  // silent: do not complain if no path is specified

    /** Get modules ********************************************************/

    // Try to obtain an interface to a MIDI-out module, use NULL if none available
    m_midiOut = DEP_Get(Depends, INI_STR_MIDIOUT, NULL, true);    // silent: do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  NewFile();

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIOut initialized ('%s')"), (LPCTSTR)(m_path)));

  return S_OK;
}

STDMETHODIMP CMIDIOut::Destroy() {
  SaveFile();

  // Release the MIDI-out module
  m_midiOut = NULL;

  // Release the runtime environment
  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("MIDIOut released")));
  RTE_Set(m_env, NULL);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IMIDIEventHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMIDIOut::HandleEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
  HRESULT hrThis = S_OK, hrThat = S_OK;

  WriteEvent(usDelta, status, data1, data2, length);

  if (m_midiOut != NULL)
    hrThat = m_midiOut->HandleEvent(usDelta, status, data1, data2, length);

  return (hrThat == S_OK ? hrThis : hrThat);
}

STDMETHODIMP CMIDIOut::HandleSysEx(LONGLONG usDelta, BYTE * data, LONG length) {
  if (data == NULL)
    return E_POINTER;

  HRESULT hrThis = S_OK, hrThat = S_OK;

  WriteSysEx(usDelta, data, length);

  if (m_midiOut != NULL)
    hrThat = m_midiOut->HandleSysEx(usDelta, data, length);

  return (hrThat == S_OK ? hrThis : hrThat);
}

STDMETHODIMP CMIDIOut::HandleRealTime(LONGLONG usDelta, BYTE data) {
  if ((data == MIDI_EVENT_SYS_RT_RESET) && (m_midHdr.dwTrackLength == SWAP_LONG(0))) {
    SaveFile();
    // Open a new file
    NewFile();
  }

  if (m_midiOut != NULL)
    return m_midiOut->HandleRealTime(usDelta, data);

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

void CMIDIOut::NewFile(void) {
  if (m_fOut.IsOpen()) {
    m_fOut.Truncate();
  } else {
    if (!m_fOut.Create(m_path + _T("\\vdms"), _T("mid"))) {
      DWORD lastError = GetLastError();
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat: Error encountered while creating MIDI file '%s'\n0x%08x - %s"), m_path + "\\vdmsXXXX.mid", lastError, (LPCTSTR)FormatMessage(lastError)));
    }
  }

  InitHeader();
  m_fOut.Write(&m_midHdr, sizeof(m_midHdr));
}

void CMIDIOut::WriteSysEx(LONGLONG usDelta, const BYTE* data, LONG length) {
  WriteEvent(usDelta, MIDI_EVENT_SYSTEM_SYSEX_ESCAPE, 0, 0, 0);

  if (m_fOut.IsOpen()) {
    if (m_dataLen + 5 + length > sizeof(m_dataBuf)) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    LONG m_dataLen_orig = m_dataLen;

    m_dataLen += GetVarLenVal((BYTE*)(&length), sizeof(length), m_dataBuf + m_dataLen);

    if (length > 0) {
      memcpy(m_dataBuf + m_dataLen, data, length);
      m_dataLen += length;
    }

    IncreaseTrackLength(m_dataLen - m_dataLen_orig);
  }
}

void CMIDIOut::WriteMeta(LONGLONG usDelta, BYTE metaCmd, const BYTE* data, LONG length) {
  WriteEvent(usDelta, MIDI_EVENT_SYSTEM_METAEVENT, metaCmd, 0, 1);

  if (m_fOut.IsOpen()) {
    if (m_dataLen + 5 + length > sizeof(m_dataBuf)) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    LONG m_dataLen_orig = m_dataLen;

    m_dataLen += GetVarLenVal((BYTE*)(&length), sizeof(length), m_dataBuf + m_dataLen);

    if (length > 0) {
      memcpy(m_dataBuf + m_dataLen, data, length);
      m_dataLen += length;
    }

    IncreaseTrackLength(m_dataLen - m_dataLen_orig);
  }
}

void CMIDIOut::WriteEvent(LONGLONG usDelta, BYTE status, BYTE data1, BYTE data2, BYTE length) {
/* TODO: get rid of following kludge by passing proper usDelta from EmuMPU401 */
usDelta = (LONGLONG)1000 * timeGetTime();

  if (m_fOut.IsOpen()) {
    if (m_midHdr.dwTrackLength == SWAP_LONG(0)) {
      m_lastTime = usDelta;
      usDelta = 0;
    } else {
      LONGLONG thisTime = usDelta;
      usDelta = (usDelta - m_lastTime) / 1000;
      m_lastTime = thisTime;
    }

    if (m_dataLen + 10 + length > sizeof(m_dataBuf)) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    LONG m_dataLen_orig = m_dataLen;

    m_dataLen += GetVarLenVal((BYTE*)(&usDelta), sizeof(usDelta), m_dataBuf + m_dataLen);

    m_dataBuf[m_dataLen++] = status;

    if (length > 0) m_dataBuf[m_dataLen++] = data1;
    if (length > 1) m_dataBuf[m_dataLen++] = data2;

    IncreaseTrackLength(m_dataLen - m_dataLen_orig);
  }
}

void CMIDIOut::SaveFile(void) {
  if (m_fOut.IsOpen()) {
    // Append a notice
    const char* message = "Captured using VDMSound (http://ntvdm.cjb.net/).  Distribution of this material may violate applicable copyright laws.";
    WriteMeta(m_lastTime, MIDI_META_TEXT, (const BYTE*)(message), strlen(message));

    // Terminate the MIDI stream
    BYTE arg = 0;
    WriteMeta(m_lastTime, MIDI_META_END_OF_TRACK, &arg, sizeof(arg));

    // Flush the buffers
    if (m_dataLen > 0) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    // Update header in file
    m_fOut.Seek(0, FILE_BEGIN);
    m_fOut.Write(&m_midHdr, sizeof(m_midHdr));

    // Close file (saves)
    m_fOut.Close();
  }
}

void CMIDIOut::InitHeader(void) {
  m_midHdr.dwStructSignature  = SWAP_LONG(0x4d546864);
  m_midHdr.dwStructLength     = SWAP_LONG(6);
  m_midHdr.wFileFormat        = SWAP_WORD(MIDI_FMT_SINGLETRACK);
  m_midHdr.wTrackCount        = SWAP_WORD(1);
  m_midHdr.wDeltaTimeTicks    = SWAP_WORD((-25 << 8) | 40); // millisecond resolution (25fps, 40 subdivisions/frame)

  m_midHdr.dwTrackSignature   = SWAP_LONG(0x4d54726b);
  m_midHdr.dwTrackLength      = SWAP_LONG(0);
}

void CMIDIOut::IncreaseTrackLength(LONG delta) {
  LONG tmp = SWAP_LONG(m_midHdr.dwTrackLength);
  tmp += delta;
  m_midHdr.dwTrackLength = SWAP_LONG(tmp);
}

int CMIDIOut::GetVarLenVal(const BYTE* valSrc, int nCount, BYTE* valDest) {
  int bitLen = GetNumSignificantBits(valSrc, nCount);
  int chunkLen = (bitLen + 6) / 7;

  for (int i = chunkLen - 1; i >= 0; i--) {
    BYTE w_byte = GetUnalignedByte(valSrc, 7 * i);

    if (i == 0) {
      w_byte &= 0x7f;
    } else {
      w_byte |= 0x80;
    }

    *(valDest++) = w_byte;
  }

  return chunkLen;
}

int CMIDIOut::GetNumSignificantBits(const BYTE* valSrc, int nCount) {
  int numBytes = 0, parBits = 1;

  for (int i = 0; i < nCount; i++) {
    if (valSrc[i]) {
      numBytes = i;
      for (int j = 0; j < 8; j++) {
        if (valSrc[i] & (1 << j)) {
          parBits = (j + 1);
        }
      }
    }
  }

  return 8 * numBytes + parBits;
}

unsigned char CMIDIOut::GetUnalignedByte(const BYTE* valSrc, int bitPos) {
  int byteIdx = bitPos / 8, bitIdx = bitPos % 8;
  BYTE lsb = valSrc[byteIdx], msb = valSrc[byteIdx + 1];

  return (lsb >> bitIdx) | (msb << (8 - bitIdx));
}
