#if !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
#define AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CRUNWITHVDMSThread class

class CRUNWITHVDMSThread : protected CWinThread
{
// Construction
private:
	CRUNWITHVDMSThread(LPCTSTR fileName);
	~CRUNWITHVDMSThread();

public:
  static HRESULT CreateThread(LPCTSTR fileName);

// Callback functions
protected:
  virtual BOOL InitInstance(void);
  virtual int Run(void);

// Member variables
protected:
  CLaunchPadSettings m_settings;
};

#endif // !defined(AFX_RUNWITHVDMSTHREAD_H__4780CB8F_E30D_4358_95E1_F5FB70C1C8E2__INCLUDED_)
