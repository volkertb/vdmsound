#define BOOL unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define HANDLE DWORD
#define LPCSTR const char far*
#define LPHANDLE HANDLE far*
#define VOID void
#define LPVOID VOID far*

#define FALSE 0
#define TRUE  1

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined __BORLANDC__ || __TURBOC__
#define _SEG(p) FP_SEG(p)
#define _stricmp stricmp
#define _strlwr strlwr
#endif

#define _VER_MAJOR 2
#define _VER_MINOR 1

#include "DOSDrv.h"



//////////////////////////////////////////////////////////////////////////////



BOOL RegisterModule(LPCSTR szVDD, LPCSTR szInit, LPCSTR szDispatch, LPHANDLE lphVDD) {
  BOOL success;

  // WARNING! szInit, szDispatch and szVDD must be on the same segment,
  //   or else NTVDM.EXE will cause problems
  assert((_SEG(szVDD) == _SEG(szDispatch)) && ((szInit == NULL) || (_SEG(szVDD) == _SEG(szInit))));

  assert(lphVDD != NULL);

  asm {
    push ax
    push bx
    push es
    push di
    push ds
    push si

    lds  si, szVDD
    lds  bx, szDispatch
    les  di, szInit

    db   0c4h
    db   0c4h
    db   58h
    db   0h

    lds  si, lphVDD
    mov  ds:[si], ax

    cmc
    lahf
    and  ah, 1
    mov  success, ah

    pop  si
    pop  ds
    pop  di
    pop  es
    pop  bx
    pop  ax
  }

  return success;
}

VOID UnRegisterModule(HANDLE hVDD) {
  asm {
    push ax

    mov  ax, hVDD

    db   0c4h
    db   0c4h
    db   58h
    db   1h

    pop  ax
  }
}

WORD DispatchCall(HANDLE hVDD, WORD cmd, LPVOID lpParam, WORD uParamLen) {
  WORD result;

  asm {
    push ax
    push bx
    push dx
    push ds
    push si

    mov  ax, hVDD
    mov  bx, cmd
    mov  dx, uParamLen
    lds  si, lpParam

    db   0c4h
    db   0c4h
    db   58h
    db   2h

    mov  result, ax

    pop  si
    pop  ds
    pop  dx
    pop  bx
    pop  ax
  }

  return result;
}



//////////////////////////////////////////////////////////////////////////////



BOOL stribegin(const char* str, const char* substr) {
  int i;

  for (i = 0; i < strlen(substr); i++) {
    if (tolower(str[i]) != tolower(substr[i])) {
      return FALSE;
    }
  } return TRUE;
}

BOOL striis(const char* str1, const char* str2) {
  return (_stricmp(str1, str2) == 0);
}

char* strrpbrk(const char *string, const char *strCharSet) {
  char* retVal = NULL;
  char* curVal;

  for (int i = 0; i < strlen(strCharSet); i++) {
    if (retVal < (curVal = strrchr(string, strCharSet[i]))) {
      retVal = curVal;
    }
  }

  return retVal;
}



//////////////////////////////////////////////////////////////////////////////



int load(LPCSTR szDllPath, LPHANDLE lphVDD, LPVOID lpParam = NULL, WORD uParamLen = 0) {
  HANDLE hVDD;
  BOOL bSuccess;
  WORD wResult;
  static char szInitFn[] = "VddInitialize";
  static char szDispFn[] = "VddDispatch";

  fprintf(stdout, "Loading '%s'...\n", szDllPath);

  bSuccess = RegisterModule(szDllPath, szInitFn, szDispFn, &hVDD);

  if (lphVDD)
    *lphVDD = hVDD;

  if (!bSuccess) {
    const char* szDllName = strrpbrk((const char*)szDllPath, ":/\\");

    if (szDllName == NULL) {
      szDllName = (const char*)szDllPath;
    } else {
      szDllName++;
    }

    switch (hVDD) {
      case 1:
        fprintf(stderr, "Error (1):\n  Could not load '%s' - DLL not found or DLL entrypoint error.\n\n", szDllName);
        break;
      case 2:
        fprintf(stderr, "Error (2):\n  Dispatch routine '%s' not found, check <%s>'s exports.\n\n", (char*)szDispFn, szDllName);
        break;
      case 3:
        fprintf(stderr, "Error (3):\n  Initialization routine '%s' not found or initialization error.\n\n", (char*)szInitFn);
        break;
      case 4:
        fprintf(stderr, "Error (4):\n  Insufficient memory available.\n\n");
        break;
      default:
        fprintf(stderr, "Unknown error %d (0x%04x).\n\n", (int)hVDD, (int)hVDD);
        break;
    }
    return -1;      // error; does not require unloading of VDD
  } else {
    wResult = DispatchCall(hVDD, CMD_VDD_INIT, lpParam, uParamLen);

    if (wResult != 0) {
      fprintf(stderr, "Error encountered during post-initialization (0x%02x).\n", (int)wResult);
      return -2;    // error; requires unloading of VDD
    }

    return 0;       // success
  }
}

int unload(HANDLE hVDD, LPVOID lpParam = NULL, WORD uParamLen = 0) {
  WORD wResult;

  wResult = DispatchCall(hVDD, CMD_VDD_DESTROY, lpParam, uParamLen);

  if (wResult != 0) {
    fprintf(stderr, "Error encountered during pre-release (0x%02x).\n", (int)wResult);
  }

  UnRegisterModule(hVDD);

  return 0;
}



//////////////////////////////////////////////////////////////////////////////



enum fnType {
  FN_DONOTHING,
  FN_HELP,
  FN_LOAD,
  FN_UNLOAD
};

int main(int argc, char** argv) {
  HANDLE hVDD = 0;
  char* szDllPath = "VDDLoader.dll";

  char INIFiles[16384] = "";
  int INIFilesLen = 0;

  fnType function = FN_LOAD;

  highvideo();
  cprintf("VDMSound DOS loader, version %d.%02d (%s)\n\r"
          "Copyright (C) 2000-2001 Vlad ROMASCANU.\n\n\r", _VER_MAJOR, _VER_MINOR, __DATE__);

  lowvideo();
  cprintf("VDMSound is covered by the GNU Public License (GPL), version 2 or later, as\n\r"
          "published by the Free Software Foundation, Inc. (http://www.fsf.org/).\n\n\r");

  normvideo();
  for (int i = 1; i < argc; i++) {
    if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
      char* arg = &(argv[i][1]);

      if (striis(arg, "h") || striis(arg, "?")) {
        function = FN_HELP;
        continue;
      }

#if 0
      if (striis(arg, "u")) {
#else
      if (stribegin(arg, "u:")) {
        sscanf(&(arg[strlen("u:")]), "%x", &hVDD);
#endif
        function = FN_UNLOAD;
        continue;
      }

      if (stribegin(arg, "i:")) {
        int INIFNameLen = strlen(&(arg[strlen("i:")]));

        if (INIFilesLen + INIFNameLen + 2 < sizeof(INIFiles)) { /* + 2 = the final two terminating NULLs */
          if (INIFNameLen > 0) {
            strcpy(&(INIFiles[INIFilesLen]), &(arg[strlen("i:")]));
            INIFilesLen += INIFNameLen + 1;
          } else {
            fprintf(stderr, "Invalid use of the switch - %s.\nPlease provide a valid file name following the switch.\n", argv[i]);
            function = FN_DONOTHING;
            break;
          }
        } else {
          fprintf(stderr, "Not enough memory.\nPlease reduce the length of your command-line parameters.\n");
          function = FN_DONOTHING;
          break;
        }

        continue;
      }

      fprintf(stderr, "Unknown switch - %s.\n", argv[i]);
      function = FN_HELP;
      break;
    } else {
      fprintf(stderr, "Invalid parameter - %s.\n", argv[i]);
      function = FN_HELP;
      break;
    }
  }

  switch (function) {
    case FN_DONOTHING:
      /* do nothing */
      break;

    case FN_HELP:
      fprintf(stdout, "Usage: DOSDRV [-i:iniFile1 [-i:iniFile2 [...]]]\n");
      break;

    case FN_LOAD:
      INIFilesLen++;
      INIFiles[INIFilesLen] = '\0';

      switch (load(szDllPath, &hVDD, INIFiles, INIFilesLen + 1)) {
        case 0:
          fprintf(stderr, "Successfully loaded & initialized (hVDD = 0x%02x).\n", hVDD);
          break;
        case -1:
          fprintf(stderr, "VDD DLL loading failed.\nCheck the README file and the FAQ for possible causes and remedies.\n");
          break;
        case -2:
          fprintf(stderr, "Could not properly initialize; unloading.\n");
          unload(hVDD);
          break;
      }

      break;

    case FN_UNLOAD:
      unload(hVDD);
      fprintf(stdout, "Unloaded (hVDD = 0x%02x).\n", hVDD);
      break;
  }

  return 0;
}
