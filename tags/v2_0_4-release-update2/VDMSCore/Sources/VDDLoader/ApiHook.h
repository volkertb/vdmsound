#ifndef __API_HOOK_H_
#define __API_HOOK_H_

FARPROC HookProcByName(HMODULE hModule, LPCSTR szImportMod, LPCSTR szImportProc, FARPROC pNewProc);

#endif // __API_HOOK_H_
