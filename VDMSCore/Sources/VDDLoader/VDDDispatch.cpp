#include "stdafx.h"

#include "DOSDrv.h"

#include "ApiHook.h"

/////////////////////////////////////////////////////////////////////////////

#include <MFCUtil.h>
#pragma comment ( lib , "MFCUtil.lib" )

/////////////////////////////////////////////////////////////////////////////

HMODULE g_hCfgLib = NULL;
LONG g_lInstanceCount = 0;

int loadConfiguration(LPVOID lpParam, WORD uParamLen);
int unloadConfiguration(LPVOID lpParam, WORD uParamLen);

void SignalVDMSStatus(bool);

/////////////////////////////////////////////////////////////////////////////

#ifdef _NTVDM_SVC

typedef VOID (WINAPI* LPFNEXITPROCESS)(UINT);

VOID WINAPI ExitProcess_Surrogate(UINT);
LPFNEXITPROCESS ExitProcess_Original = NULL;

#endif //_NTVDM_SVC

/////////////////////////////////////////////////////////////////////////////

typedef HRESULT (WINAPI* LPFNCFGINITIALIZE)(char*);
typedef HRESULT (WINAPI* LPFNCFGDESTROY)(void);

/////////////////////////////////////////////////////////////////////////////


//
// Initialization routine; called right after NTVDM.EXE or equivalent loads
//   VDDLoader.DLL in its memory space
//
STDAPI_(void) VddInitialize(void) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  setCF(0);
}

//
// Dispatch routine; allows DOS applications to send commands to the 32-bit
//   environemnt.
// Parameters:
//   BX    = Command ID
//   DS:SI = Pointer to user-data buffer
//   DX    = User-data buffer length
//
STDAPI_(void) VddDispatch(void) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  WORD dispCmd     = getBX();
  WORD paramSeg    = getDS();
  WORD paramOffset = getSI();
  WORD paramLen    = getDX();

  LPVOID param = GetVDMPointer(MAKELONG(paramOffset, paramSeg), paramLen, FALSE);

  int retVal;

  switch (dispCmd) {
    case CMD_VDD_INIT:
      retVal = loadConfiguration(param, paramLen);
      break;

    case CMD_VDD_DESTROY:
      retVal = unloadConfiguration(param, paramLen);
      break;

    default:
      retVal = -1;
      break;
  }

  FreeVDMPointer(MAKELONG(paramOffset, paramSeg), paramLen, param, FALSE);

  setAX(retVal);
}



/////////////////////////////////////////////////////////////////////////////



//
// Loads the VDMConfig DLL, and calls its initialization routine in order to
//   parse the configuration files and in turn load the sub-modules that
//   provide the emulation
//
int loadConfiguration(LPVOID lpParam, WORD uParamLen) {
  try {
    // Check if first instance
    if ((++g_lInstanceCount) > 1) {
      MessageBox(FormatMessage(_T("Program is already loaded (VDDLoader.DLL).%n%nPlease unload before attempting to reload.")),
                 FormatMessage(_T("VDD Error")),
                 MB_OK, MB_ICONERROR);
      return 0x80;        // Initialization already occured (1)?
    }

    if (g_hCfgLib != NULL) {
      MessageBox(FormatMessage(_T("Program is already loaded (VDMConfig.DLL).%n%nPlease unload before attempting to reload.")),
                 FormatMessage(_T("VDD Error")),
                 MB_OK, MB_ICONERROR);
      return 0x81;        // Initialization already occured (2)?
    }

    // Load library
    g_hCfgLib = LoadLibrary(_T("VDMConfig.dll"));

    if (g_hCfgLib == NULL) {
      DWORD lastError = GetLastError();
      MessageBox(FormatMessage(_T("Failed to load configuration library (VDMConfig.DLL).%n%nLast error reported by Windows:%n0x%1!08x! - %2%n%nPlease make sure that VDMConfig.DLL is in the current directory or that its location is in the PATH, and that it is a valid image."), false, lastError, (LPCTSTR)FormatMessage(lastError)),
                 FormatMessage(_T("VDD Error")),
                 MB_OK, MB_ICONERROR);
      return 0x90;        // Could not load DLL
    }

    // Obtain initialization information
    char* INIFiles;

    if ((lpParam == NULL) || (((char*)lpParam)[0] == '\0')) {
      INIFiles = "VDMS.INI";   // use a default value
    } else {
      INIFiles = (char*)lpParam;  // information provided by DOS loader
    }

    // Try to invoke initialization code
    LPFNCFGINITIALIZE lpfnInitProc;

    lpfnInitProc = (LPFNCFGINITIALIZE)GetProcAddress(g_hCfgLib, "CfgInitialize");

    if (lpfnInitProc == NULL) {
      DWORD lastError = GetLastError();
      MessageBox(FormatMessage(_T("Failed to locate exported function 'CfgInitialize' in configuration library (VDMConfig.DLL).%n%nLast error reported by Windows:%n0x%1!08x! - %2%n%nPlease make sure that VDMConfig.DLL is a valid image."), false, lastError, (LPCTSTR)FormatMessage(lastError)),
                 FormatMessage(_T("VDD Error")),
                 MB_OK, MB_ICONERROR);
      return 0xa0;        // Could not find procedure in DLL
    }

    if (FAILED((*lpfnInitProc)(INIFiles))) {
      return 0xf0;        // Initialization failed, or aborted by user
    }

    // Successful load & initialization
    SignalVDMSStatus(true);

#ifdef _NTVDM_SVC

    // Hook ExitProcess, for clean shutdowns (hook automatically calls
    //   unloadConfiguration when NTVDM.EXE ends normally)
    ExitProcess_Original = (LPFNEXITPROCESS)HookProcByName(GetModuleHandleA("NTVDM.EXE"), "KERNEL32.DLL", "ExitProcess", (FARPROC)ExitProcess_Surrogate);

    if (ExitProcess_Original == NULL) {
      DWORD lastError = GetLastError();
      if (MessageBox(FormatMessage(_T("Failed to hook ExitProcess() in KERNEL32.DLL.  This means that the VDD will be unable to automatically unload in a clean manner when the console window closes, which will probably lead to a protection fault.%n%nLast error reported by Windows:%n0x%1!08x! - %2%n%nDo you want to continue?"), false, lastError, (LPCTSTR)FormatMessage(lastError)),
                     FormatMessage(_T("VDD Error")),
                     MB_YESNO|MB_DEFBUTTON1, MB_ICONERROR) == IDNO)
      {
        return 0xb0;        // Could not hook function
      }
    }

#endif //_NTVDM_SVC

  } catch (...) {
    MessageBox(FormatMessage(_T("Unhandled exception encountered while loading")),
               FormatMessage(_T("Fatal Error")),
               MB_OK, MB_ICONERROR);
    return 0xff;            // Catastrophic failure
  }

  return 0;                 // Success
}


//
// Unloads the VDMConfig DLL, but not before calling its initialization
//   cleanup routine in order to unload the sub-modules that provide the
//   emulation
//
int unloadConfiguration(LPVOID lpParam, WORD uParamLen) {
  int retVal = 0;

  try {
    // Unloading old instance
    g_lInstanceCount--;
    // Check if first instance
    if (g_lInstanceCount > 0)
      return 0;           // Don't need to do anything more if we were not the first instance
    if (g_lInstanceCount < 0) {
      g_lInstanceCount = 0;
      return 0x88;        // Initialization did not occur (1)?
    }

    if (g_hCfgLib == NULL)
      return 0x89;        // Initialization did not occur (2), or DLL load failed?

    // Try to invoke cleanup code
    LPFNCFGDESTROY lpfnDestroyProc;

    lpfnDestroyProc = (LPFNCFGDESTROY)GetProcAddress(g_hCfgLib, "CfgDestroy");

    if (lpfnDestroyProc == NULL) {
      DWORD lastError = GetLastError();
      MessageBox(FormatMessage(_T("Failed to locate exported function 'CfgDestroy' in configuration library (VDMConfig.DLL).%n%nLast error reported by Windows:%n0x%1!08x! - %2%n%nPlease make sure that VDMConfig.DLL is a valid image."), false, lastError, (LPCTSTR)FormatMessage(lastError)),
                 FormatMessage(_T("VDD Error")),
                 MB_OK, MB_ICONERROR);
      retVal = 0xa8;      // Could not find procedure in DLL, but proceed with unloading the library before returning
    } else {
      (*lpfnDestroyProc)();
    }

    if (!FreeLibrary(g_hCfgLib)) {
      retVal = 0x98;      // Could not unload DLL
    }

    // Note: No need to remove the ExitProcess hook because the process
    //  is about to exit anyway

    // Clean up handles
    g_hCfgLib = NULL;

    // Done unloading
    SignalVDMSStatus(false);
  } catch (...) {
    MessageBox(FormatMessage(_T("Unhandled exception encountered while unloading")),
               FormatMessage(_T("Fatal Error")),
               MB_OK, MB_ICONERROR);
    return 0xff;            // Catastrophic failure
  }

  return retVal;
}



/////////////////////////////////////////////////////////////////////////////



//
// Give audio feedback with regard to VDMS's status (loading/unloading).
//
void SignalVDMSStatus(bool isLoaded) {

# if 0

  if (isLoaded) {
    Beep(1000,50);
    Beep(2000,60);
    Beep(4000,80);
  } else {
    Beep(4000,50);
    Beep(2000,60);
    Beep(1000,80);
  }

# endif

}



/////////////////////////////////////////////////////////////////////////////


#ifdef _NTVDM_SVC

//
// Automatically unloads the configuration module (and, implictly, all
// emulation modules) when the process is about to exit.
//
VOID WINAPI ExitProcess_Surrogate(UINT uExitCode) {
  unloadConfiguration(NULL, 0);

  if ((ExitProcess_Original != NULL) &&                 // avoid protection fault
      (ExitProcess_Original != ExitProcess_Surrogate))  // avoid infinite recursion
  {
    (*ExitProcess_Original)(uExitCode);
  }
}

#endif //_NTVDM_SVC
