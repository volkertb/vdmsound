// WaveOut.cpp : Implementation of CWaveOut
#include "stdafx.h"
#include "DiskWriter.h"
#include "WaveOut.h"

/////////////////////////////////////////////////////////////////////////////

/* TODO: put these in a .mc file or something */
#define MSG_ERR_IO_OPEN         _T("An error was encountered while attempting to create or to open file '%1' for writing.%n%nLast error reported by Windows:%n0x%2!08x! - %3%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_WAVEOUT       L"WaveOut"
#define INI_STR_PATH          L"Path"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

#ifdef MAKEFOURCC
#undef MAKEFOURCC
#endif // MAKEFOURCC

#ifdef WAVE_FORMAT_PCM
#undef WAVE_FORMAT_PCM
#endif // WAVE_FORMAT_PCM

#define WAVE_FORMAT_PCM 1

#define MAKEFOURCC(ch0, ch1, ch2, ch3)  \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |  \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ));

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

    // Try to obtain an interface to a Wave-out module, use NULL if none available
    m_waveOut = DEP_Get(Depends, INI_STR_WAVEOUT, NULL, true);    // silent: do not complain if no such module available
  } catch (_com_error& ce) {
    SetErrorInfo(0, ce.ErrorInfo());
    return ce.Error();                // Propagate the error
  }

  InitHeader(0, 0, 0);

  RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("WaveOut initialized ('%s')"), (LPCTSTR)(m_path)));

  return S_OK;
}

STDMETHODIMP CWaveOut::Destroy() {
  SaveFile();

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
  // Check if the file has to be (re)opened following a change in
  //  playback format (if the file was
  //  the device to be closed then opened again with the new format
  if ((m_wavHdr.nChannels != channels) ||
      (m_wavHdr.nSamplesPerSec != samplesPerSec) ||
      (m_wavHdr.wBitsPerSample != bitsPerSample))
  {
    // If there is any data in the current file, save and close it
    if (m_wavHdr.dwDataChunkSize > 0) {
      SaveFile();
    }

    // Open a new file or reuse current, empty file
    NewFile(channels, samplesPerSec, bitsPerSample);
  }

  AddCue();

  if (m_waveOut != NULL)
    return m_waveOut->SetFormat(channels, samplesPerSec, bitsPerSample);

  return S_OK;
}

STDMETHODIMP CWaveOut::PlayData(BYTE * data, LONG length, DOUBLE * load) {
  if (data == NULL)
    return E_POINTER;

  WriteData(data, length);

  if (m_waveOut != NULL)
    return m_waveOut->PlayData(data, length, load);

  *load = 1.00;

  return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////

void CWaveOut::NewFile(WORD channels, DWORD samplesPerSec, WORD bitsPerSample) {
  if (m_fOut.IsOpen()) {
    m_fOut.Truncate();
  } else {
    if (!m_fOut.Create(m_path + _T("\\vdms"), _T("wav"))) {
      DWORD lastError = GetLastError();
      RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("SetFormat: Error encountered while creating wave file '%s'\n0x%08x - %s"), m_path + "\\vdmsXXXX.wav", lastError, (LPCTSTR)FormatMessage(lastError)));
    }
  }

  CSingleLock lock(&m_lock, TRUE);    // protect m_wavHdr

  InitHeader(channels, samplesPerSec, bitsPerSample);
  m_fOut.Write(&m_wavHdr, sizeof(m_wavHdr));
}

void CWaveOut::AddCue(void) {
  if ((m_cues.GetSize() < 1) || (m_wavHdr.dwDataChunkSize != m_cues.GetAt(m_cues.GetSize() - 1))) {
    m_cues.Add(m_wavHdr.dwDataChunkSize);
  }
}

void CWaveOut::WriteData(BYTE * data, LONG length) {
  if (m_fOut.IsOpen()) {
    CSingleLock lock(&m_lock, TRUE);  // protect m_wavHdr, m_dataLen

    if (m_dataLen + length > sizeof(m_dataBuf)) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    memcpy(m_dataBuf + m_dataLen, data, length);
    m_dataLen += length;

    m_wavHdr.dwDataChunkSize += length;
  }
}

void CWaveOut::SaveFile(void) {
  if (m_fOut.IsOpen()) {
    CSingleLock lock(&m_lock, TRUE);  // protect m_wavHdr, m_dataLen

    // Flush the buffers
    if (m_dataLen > 0) {
      m_fOut.Write(m_dataBuf, m_dataLen);
      m_dataLen = 0;
    }

    // Write out the cue list
    AddCue();

    m_wavTail.dwCueChunkID    = MAKEFOURCC('c','u','e',' ');
    m_wavTail.dwCueChunkSize  = sizeof(DWORD) + (6 * sizeof(DWORD)) * m_cues.GetSize();
    m_wavTail.dwCuePoints     = m_cues.GetSize();

    m_fOut.Write(&m_wavTail, sizeof(m_wavTail));

    for (int i = 0; i < m_cues.GetSize(); i++) {
      DWORD dwIdentifier    = i;
      DWORD dwPosition      = i;
      DWORD fccChunk        = MAKEFOURCC('d','a','t','a');
      DWORD dwChunkStart    = 0;
      DWORD dwBlockStart    = 0;
      DWORD dwSampleOffset  = m_cues.GetAt(i);

      m_fOut.Write(&dwIdentifier,   sizeof(dwIdentifier));
      m_fOut.Write(&dwPosition,     sizeof(dwPosition));
      m_fOut.Write(&fccChunk,       sizeof(fccChunk));
      m_fOut.Write(&dwChunkStart,   sizeof(dwChunkStart));
      m_fOut.Write(&dwBlockStart,   sizeof(dwBlockStart));
      m_fOut.Write(&dwSampleOffset, sizeof(dwSampleOffset));
    }

    m_cues.RemoveAll();

    // Finalize headers
    m_wavHdr.dwRiffChunkSize =
      1 * sizeof(DWORD) +                             // length of 'RIFF' chunk
      2 * sizeof(DWORD) + m_wavHdr.dwFmtChunkSize +   // length of 'fmt ' chunk + chunk header
      2 * sizeof(DWORD) + m_wavHdr.dwDataChunkSize +  // length of 'data' chunk + chunk header
      2 * sizeof(DWORD) + m_wavTail.dwCueChunkSize;   // length of 'cue ' chunk + chunk header

    // Pad file, if necessary
    if (m_wavHdr.dwRiffChunkSize & 0x1) {
      char pad = 0x00;
      m_fOut.Write(&pad, sizeof(pad));
    }

    // Update header in file
    m_fOut.Seek(0, FILE_BEGIN);
    m_fOut.Write(&m_wavHdr, sizeof(m_wavHdr));

    // Close file (saves)
    m_fOut.Close();
  }
}

void CWaveOut::InitHeader(WORD channels, DWORD samplesPerSec, WORD bitsPerSample) {
  m_wavHdr.dwRiffChunkID    = MAKEFOURCC('R','I','F','F');
  m_wavHdr.dwRiffChunkSize  = 36;
  m_wavHdr.dwRiffFormat     = MAKEFOURCC('W','A','V','E');

  m_wavHdr.dwFmtChunkID     = MAKEFOURCC('f','m','t',' ');
  m_wavHdr.dwFmtChunkSize   = 16;
  m_wavHdr.wFormatTag       = WAVE_FORMAT_PCM;
  m_wavHdr.nChannels        = channels;
  m_wavHdr.nSamplesPerSec   = samplesPerSec;
  m_wavHdr.nAvgBytesPerSec  = channels * samplesPerSec * bitsPerSample / 8;
  m_wavHdr.nBlockAlign      = channels * bitsPerSample / 8;
  m_wavHdr.wBitsPerSample   = bitsPerSample;

  m_wavHdr.dwDataChunkID    = MAKEFOURCC('d','a','t','a');
  m_wavHdr.dwDataChunkSize  = 0;
}
