// TipFileReader.cpp: implementation of the CTipFileReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TipOfDay.h"
#include "TipFileReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTipFileReader::CTipFileReader() : m_title(_T(""))
{

}

CTipFileReader::~CTipFileReader()
{

}


//////////////////////////////////////////////////////////////////////



DWORD CTipFileReader::LoadTips(LPCTSTR lpszFileName) {
  try {
    m_tips.RemoveAll();
    m_title.Empty();

    BOOL isEOF = FALSE;
    CStdioFile tipsFile(lpszFileName, CFile::modeRead | CFile::typeText);
    CString currentTip, currentLine;

    do {
      currentLine.Empty();
      isEOF = !tipsFile.ReadString(currentLine);

      if (currentLine.IsEmpty()) {
        if (!currentTip.IsEmpty()) {
          currentTip.TrimRight(_T("\r\n"));

          if (m_title.IsEmpty()) {
            m_title = currentTip;
          } else {
            m_tips.Add(currentTip);
          }

          currentTip.Empty();
        }
      } else {
        currentTip += currentLine;
        currentTip += _T("\r\n");
      }
    } while (!isEOF);

    return ERROR_SUCCESS;
  } catch (CFileException* cfe) {
    return cfe->m_lOsError;
  } catch (CMemoryException* /*cme*/) {
    return ERROR_NOT_ENOUGH_MEMORY;
  }
}

int CTipFileReader::GetNumTips(void) {
  return m_tips.GetSize();
}

CString CTipFileReader::GetTip(int index) {
  if ((index >= 0) && (index < m_tips.GetSize())) {
    return m_tips.GetAt(index);
  } else {
    return CString(_T(""));
  }
}

CString CTipFileReader::GetTitle(void) {
  return m_title;
}
