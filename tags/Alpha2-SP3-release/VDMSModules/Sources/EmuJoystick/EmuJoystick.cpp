// EmuJoystick.cpp : Implementierung der DLL-Exporte.


// Hinweis: Proxy/Stub-Information
//      Um eine eigene Proxy/Stub-DLL zu erstellen, 
//      führen Sie nmake -f EmuJoystickps.mk im Projektverzeichnis aus.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "EmuJoystick.h"

#include "EmuJoystick_i.c"
#include "EmuJoysticks.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_EmuJoysticks, CEmuJoysticks)
END_OBJECT_MAP()

class CEmuJoystickApp : public CWinApp
{
public:

// Überladungen
	// Überladungen für vom Klassen-Assistenten generierte virtuelle Funktionen
	//{{AFX_VIRTUAL(CEmuJoystickApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEmuJoystickApp)
		// HINWEIS - der Klassen-Assistent fügt hier Member-Funktionen hinzu oder löscht sie.
		//    BEARBEITEN SIE NICHT, was Sie in diesen generierten Code-Blöcken sehen !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CEmuJoystickApp, CWinApp)
	//{{AFX_MSG_MAP(CEmuJoystickApp)
		// HINWEIS - der Klassen-Assistent fügt hier Umsetzungsmakros hinzu oder löscht sie.
		//    BEARBEITEN SIE NICHT, was Sie in diesen generierten Code-Blöcken sehen !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CEmuJoystickApp theApp;

BOOL CEmuJoystickApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_EMUJOYSTICKLib);
    return CWinApp::InitInstance();
}

int CEmuJoystickApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Verwendet, um zu entscheiden, ob die DLL von OLE aus dem Speicher entfernt werden kann

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Liefert eine Klassenfabrik zurück, um ein Objekt des angeforderten Typs anzulegen

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Fügt der Systemregistrierung Einträge hinzu

STDAPI DllRegisterServer(void)
{
    // Registriert Objekt, Typelib und alle Schnittstellen in Typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Entfernt Einträge aus der Systemregistrierung

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


