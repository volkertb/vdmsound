// JoystickCtl.cpp : Implementierung von CJoystickCtl
#include "stdafx.h"
#include "EmuJoystick.h"
#include "JoystickCtl.h"

/* TODO: put these in a .mc file or something */
#define MSG_ERR_INTERFACE _T("The dependency module '%1' does not support the '%2' interface.%0")

/////////////////////////////////////////////////////////////////////////////

#define INI_STR_VDMSERVICES   L"VDMSrv"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

#include <VDMUtil.h>
#pragma comment ( lib , "VDMUtil.lib" )

#pragma comment ( lib , "winmm.lib" )


/////////////////////////////////////////////////////////////////////////////
// CJoystickCtl

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IVDMBasicModule,
		&IID_IIOHandler,
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

STDMETHODIMP CJoystickCtl::Init(IUnknown * configuration) {
	if (configuration == NULL)
		return E_POINTER;

	HRESULT hr;

	IVDMQUERYLib::IVDMQueryDependenciesPtr Depends;   // Dependency query object
	IVDMQUERYLib::IVDMQueryConfigurationPtr Config;   // Configuration query object

	// Grab a copy of the runtime environment (useful for logging, etc.)
	RTE_Set(m_env, configuration);

	// query how many joysticks are installed
	m_NumJoysticks = joyGetNumDevs();
	if (m_NumJoysticks == 0) { //no joystick -> do nothing
		return S_OK;
	}

	// emulate 1 or 2 joysticks?
	m_NumJoysticks = m_NumJoysticks >= 2 ? 2 : 1;

	// Initialize configuration and VDM services
	try {
		// Obtain the Query objects (for intialization purposes)
		Depends    = configuration; // Dependency query object
		Config     = configuration; // Configuration query object

		// Obtain VDM Services instance
		IUnknownPtr VDMServices
		           = Depends->Get(INI_STR_VDMSERVICES);
		m_BaseSrv  = VDMServices;   // Base services (registers, interrupts, etc)
		m_IOSrv    = VDMServices;   // I/O services (I/O port hooks)

		if (m_BaseSrv == NULL)
			return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMBaseServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);
		if (m_IOSrv == NULL)
			return AtlReportError(GetObjectCLSID(), (LPCTSTR)::FormatMessage(MSG_ERR_INTERFACE, /*false, NULL, 0, */false, (LPCTSTR)CString(INI_STR_VDMSERVICES), _T("IVDMIOServices")), __uuidof(IVDMBasicModule), E_NOINTERFACE);

	} catch (_com_error& ce) {
		SetErrorInfo(0, ce.ErrorInfo());
		return ce.Error();          // Propagate the error
	}

	IVDMSERVICESLib::IIOHandler* pHandler = NULL; // IIOHandler interface to <this>

	// Register the I/O handlers
	try {
		// Obtain a COM IIOHandler interface on this C++ object
		if (FAILED(hr = QueryInterface(__uuidof(IVDMSERVICESLib::IIOHandler), (void**)(&pHandler))))
			throw _com_error(hr);     // Failure

		// Add this object as an I/O handler on the specified port range
		m_IOSrv->AddIOHook(0x201, 1, IVDMSERVICESLib::OP_SINGLE_BYTE, IVDMSERVICESLib::OP_SINGLE_BYTE, pHandler);

		pHandler->Release();        // Take back the AddRef in QueryInterface above
	} catch (_com_error& ce) {
		if (pHandler != NULL)
			pHandler->Release();      // Release the (unused) interface

		SetErrorInfo(0, ce.ErrorInfo());
		return ce.Error();          // Propagate the error
	}

	RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("JoystickCtl initialized")));

	return S_OK;
}

STDMETHODIMP CJoystickCtl::Destroy() {
	// Release the VDM Services module
	m_IOSrv   = NULL;
	m_BaseSrv = NULL;

	// Release the runtime environment
	RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_INFORMATION, Format(_T("JoystickCtl  released")));
	RTE_Set(m_env, NULL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IIOHandler
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::HandleINB(USHORT inPort, BYTE * data) {
	if (data == NULL)
		return E_POINTER;

	if (inPort != 0x201) { //not the joystick port
		*data = 0xFF;
		return E_FAIL;
	}

	// return button state / axis bits
	/* format of the byte to be returned:
	                      +-------------------------------+
	                      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	                      +-------------------------------+
	                        |   |   |   |   |   |   |   |
	Joystick B, Button 2 ---+   |   |   |   |   |   |   +--- Joystick A, X Axis
	Joystick B, Button 1 -------+   |   |   |   |   +------- Joystick A, Y Axis
	Joystick A, Button 2 -----------+   |   |   +----------- Joystick B, X Axis
	Joystick A, Button 1 ---------------+   +--------------- Joystick B, Y Axis

	for now, the joystick values are only read when the program
	writes to the port, otherwise we would have to call joyGetPos
	to often. maybe we should update the status of the buttons every
	other call or in less frequent interval with the reads....

	*/
//	*data = m_J2Button2 << 7 ||
//			m_J2Button1 << 6 ||
//			m_J1Button2 << 5 ||
//			m_J1Button1 << 4;

	*data = 0;

	if (m_J2Button2 != 0)
		*data |= 0x80;

	if (m_J2Button1 != 0)
		*data |= 0x40;

	if (m_J1Button2 != 0)
		*data |= 0x20;

	if (m_J1Button1 != 0)
		*data |= 0x10;

	if (m_J2YCountdown != 0) {
		m_J2YCountdown--;
		*data |= 0x08;
	}

	if (m_J2XCountdown != 0) {
		m_J2XCountdown--;
		*data |= 0x04;
	}

	if (m_J1YCountdown != 0) {
		m_J1YCountdown--;
		*data |= 0x02;
	}

	if (m_J1XCountdown != 0) {
		m_J1XCountdown--;
		*data |= 0x01;
	}
//	RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("Byte sent: %x"), *data));
	return S_OK;
}

STDMETHODIMP CJoystickCtl::HandleOUTB(USHORT outPort, BYTE data) {
	JOYINFO info;
	if (outPort != 0x201)
		return E_FAIL;

	// ! WARNING !
	// i assume that joystick coordinates will be in the range of
	// 0 to 65535 (the variables are named w*pos -> they should
	// be words

	// the range ist mapped to 0 - 900 so old games don't get
	// problems because of to high values
	RTE_RecordLogEntry(m_env, IVDMQUERYLib::LOG_ERROR, Format(_T("written to joystick port")));

	switch (m_NumJoysticks) {
	case 2:
		if (joyGetPos(JOYSTICKID2, &info) == JOYERR_NOERROR) {
			m_J2Button1 = (info.wButtons & JOY_BUTTON1) != 0? 1 : 0;
			m_J2Button2 = (info.wButtons & JOY_BUTTON2) != 0? 1 : 0;
			m_J2XCountdown = (info.wXpos * 900) / 65535;
			m_J2YCountdown = (info.wYpos * 900) / 65535;
		} else {
			m_J2Button1 = 0;
			m_J2Button2 = 0;
			m_J2XCountdown = 0;
			m_J2YCountdown = 0;
		}
	case 1:
		if (joyGetPos(JOYSTICKID1, &info) == JOYERR_NOERROR) {
			m_J1Button1 = (info.wButtons & JOY_BUTTON1) != 0? 1 : 0;
			m_J1Button2 = (info.wButtons & JOY_BUTTON2) != 0? 1 : 0;
			m_J1XCountdown = (info.wXpos * 900) / 65535;
			m_J1YCountdown = (info.wYpos * 900) / 65535;
		} else {
			m_J1Button1 = 0;
			m_J1Button2 = 0;
			m_J1XCountdown = 0;
			m_J1YCountdown = 0;
		}
	}
	return S_OK;		
}


// ** BEGIN not implemented ** //////////////////////////////////////////////

STDMETHODIMP CJoystickCtl::HandleINW(USHORT inPort, USHORT * data) {
	if (data == NULL) return E_POINTER;
	*data = -1;
	return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleINSB(USHORT inPort, BYTE * data, USHORT count, DIR_T direction) {
	if (data == NULL) return E_POINTER;
	memset(data, -1, count * sizeof(data[0]));
	return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleINSW(USHORT inPort, USHORT * data, USHORT count, DIR_T direction) {
	if (data == NULL) return E_POINTER;
	memset(data, -1, count * sizeof(data[0]));
	return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTW(USHORT outPort, USHORT data) {
	return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTSB(USHORT outPort, BYTE * data, USHORT count, DIR_T direction) {
	if (data == NULL) return E_POINTER;
	return E_NOTIMPL;
}
STDMETHODIMP CJoystickCtl::HandleOUTSW(USHORT outPort, USHORT * data, USHORT count, DIR_T direction) {
	if (data == NULL) return E_POINTER;
	return E_NOTIMPL;
}

// ** END not implemented ** ////////////////////////////////////////////////
