// RUNWITHVDMSDispatcher.h: interface for the CRUNWITHVDMSDispatcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNWITHVDMSDISPATCHER_H__9D630F6C_BE3C_4E7C_8BA1_699264F58B3A__INCLUDED_)
#define AFX_RUNWITHVDMSDISPATCHER_H__9D630F6C_BE3C_4E7C_8BA1_699264F58B3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRUNWITHVDMSDispatcher  
{
public:
	CRUNWITHVDMSDispatcher(CString fileName);
	virtual ~CRUNWITHVDMSDispatcher();

public:
  HRESULT RunWithVdms(void);
  HRESULT RunWithVdms_Wizard(void);

protected:
  CString m_fileName;
};

#endif // !defined(AFX_RUNWITHVDMSDISPATCHER_H__9D630F6C_BE3C_4E7C_8BA1_699264F58B3A__INCLUDED_)
