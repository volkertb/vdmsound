#include "stdafx.h"
#include "DiskUtil.h"

#define MAX_FILES 9999        // how many files can exist

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSequentialFile::CSequentialFile() :
  m_lastSequence(-1),
  m_hFile(INVALID_HANDLE_VALUE),
  m_strFileName(_T(""))
{
}

CSequentialFile::~CSequentialFile()
{
  if (m_hFile != INVALID_HANDLE_VALUE) {
    Close();
  }
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

//
// Creates a sequential file with the given prefix, extension and
//  attributes
//
BOOL CSequentialFile::Create(
  LPCTSTR szPrefix,
  LPCTSTR szExtension,
  DWORD dwDesiredAccess,
  DWORD dwShareMode,
  DWORD dwFlagsAndAttributes)
{
  ASSERT(m_hFile == INVALID_HANDLE_VALUE);

  if (m_hFile != INVALID_HANDLE_VALUE)
    return FALSE;

  // Try to generate a sequential file name between 0 and MAX_FILES
  for (int i = m_lastSequence + 1; i <= MAX_FILES; i++) {
    m_strFileName.Format(_T("%s%04d.%s"), szPrefix, i, szExtension);

    if ((m_hFile = CreateFile(m_strFileName, dwDesiredAccess, dwShareMode, NULL, CREATE_NEW, dwFlagsAndAttributes, NULL)) != INVALID_HANDLE_VALUE) {
      m_lastSequence = i;
      break;
    }
  }

  return (m_hFile != INVALID_HANDLE_VALUE);
}

//
// Truncates the file to the desired size
//
BOOL CSequentialFile::Truncate(
  DWORD newSize)
{
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  if (SetFilePointer(m_hFile, newSize, NULL, FILE_BEGIN) == (DWORD)(-1))
    return FALSE;

  return SetEndOfFile(m_hFile);
}

//
// Seeks to the desired position
//
BOOL CSequentialFile::Seek(
  LONG lDistanceToMove,
  DWORD dwMoveMethod)
{
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  if (SetFilePointer(m_hFile, lDistanceToMove, NULL, dwMoveMethod) == (DWORD)(-1))
    return FALSE;

  return TRUE;
}

//
// Writes raw data to the sequential file
//
DWORD CSequentialFile::Write(
  LPCVOID lpBuffer,
  DWORD nNumberOfBytesToWrite)
{
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  DWORD numberOfBytesWritten;

  if (!WriteFile(m_hFile, lpBuffer, nNumberOfBytesToWrite, &numberOfBytesWritten, NULL))
    return 0;

  return numberOfBytesWritten;
}

//
// Closes the sequential file
//
BOOL CSequentialFile::Close(void) {
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  if (CloseHandle(m_hFile)) {
    m_hFile = INVALID_HANDLE_VALUE;
    return TRUE;
  } else {
    return FALSE;
  }
}

//
// Returns whether the sequential file is open or not
//
BOOL CSequentialFile::IsOpen(void) {
  return (m_hFile != INVALID_HANDLE_VALUE);
}

//
// Returns the name of the sequential file
//
CString CSequentialFile::GetFileName(void) {
  ASSERT(m_hFile != INVALID_HANDLE_VALUE);

  if (m_hFile == INVALID_HANDLE_VALUE)
    return _T("");

  return m_strFileName;
}
