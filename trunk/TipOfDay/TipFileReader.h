// TipFileReader.h: interface for the CTipFileReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIPFILEREADER_H__1372E2E0_8867_11D5_9D78_00A024112F81__INCLUDED_)
#define AFX_TIPFILEREADER_H__1372E2E0_8867_11D5_9D78_00A024112F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTipFileReader  
{
public:
	CTipFileReader();
	virtual ~CTipFileReader();

public:
  DWORD LoadTips(LPCTSTR lpszFileName);
  int GetNumTips(void);
  CString GetTip(int index);
  CString GetTitle(void);

protected:
  CArray<CString,CString&> m_tips;
  CString m_title;
};

#endif // !defined(AFX_TIPFILEREADER_H__1372E2E0_8867_11D5_9D78_00A024112F81__INCLUDED_)
