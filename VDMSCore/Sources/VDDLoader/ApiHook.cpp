#include "stdafx.h"
#include "ApiHook.h"

#define MK_POINTER(base, offset) ((void*)(((ULONG)(base)) + ((ULONG)(offset))))

//
// Gets the import descriptor for the requested module.  If the module
//  is not imported in hModule, NULL is returned.
//
static PIMAGE_IMPORT_DESCRIPTOR GetNamedImportDescriptor(
  HMODULE hModule,        // The module to hook in
  LPCSTR  szImportMod )   // The module name to get the import descriptor for
{
  ASSERT(hModule != NULL);
  ASSERT(szImportMod != NULL);

  // Check params
  if ((hModule == NULL) || (szImportMod == NULL)) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
  }

  PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)(hModule);

  // Check for DOS executable (MZ) header
  if (IsBadReadPtr(pDOSHeader, sizeof(*pDOSHeader)) || (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)) {
    ASSERT(FALSE);
    SetLastError(ERROR_INVALID_HANDLE);
    return NULL;
  }

  // Get the PE header.
  PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(MK_POINTER(hModule, pDOSHeader->e_lfanew));

  // Check for PE image header
  if (IsBadReadPtr(pNTHeader, sizeof(*pNTHeader)) || (pNTHeader->Signature != IMAGE_NT_SIGNATURE)) {
    ASSERT(FALSE);
    SetLastError(ERROR_INVALID_HANDLE);
    return NULL;
  }

  // Get the pointer to the imports section.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)(MK_POINTER(hModule, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

  // Check if an imports section is present
  if (pImportDesc == NULL) {
    return NULL;
  }

  // Look for module that macthes szImportMod
  for (; pImportDesc->Name != NULL; pImportDesc++) {
    LPCSTR szCurrMod = (LPCSTR)(MK_POINTER(hModule, pImportDesc->Name));

    if (stricmp(szImportMod, szCurrMod) == 0) {
      return pImportDesc;
    }
  }

  return NULL;
}

//
//
//
FARPROC HookProcByName(
  HMODULE hModule,
  LPCSTR  szImportMod,
  LPCSTR  szImportProc,
  FARPROC pNewProc)
{
  ASSERT(hModule != NULL);
  ASSERT(szImportMod != NULL);
  ASSERT(szImportProc != NULL);
  ASSERT(pNewProc != NULL);

  // Check params
  if ((hModule == NULL) || (szImportMod == NULL) ||
      (szImportProc == NULL) || (pNewProc == NULL))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
  }

  // Get the specific import descriptor.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = GetNamedImportDescriptor(hModule, szImportMod);

  if (pImportDesc == NULL) {
    return NULL;    // The requested module was not imported
  }

  // Get the original thunk information for this DLL.  I cannot use
  //  the thunk information stored in the pImportDesc->FirstThunk
  //  because the that is the array that the loader has already
  //  bashed to fix up all the imports.  This pointer gives us acess
  //  to the function names.
  PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)(MK_POINTER(hModule, pImportDesc->OriginalFirstThunk));

  // Get the array pointed to by the pImportDesc->FirstThunk.  This is
  //  where I will do the actual bash.
  PIMAGE_THUNK_DATA pRealThunk = (PIMAGE_THUNK_DATA)(MK_POINTER(hModule, pImportDesc->FirstThunk));

  // Look for function that macthes szImportProc
  for (; pOrigThunk->u1.Function != NULL; pOrigThunk++, pRealThunk++) {
    // Only look at functions that are imported by name, not ordinal
    if ((pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) == IMAGE_ORDINAL_FLAG)
      continue;

    // Get the name of this imported function
    PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)(MK_POINTER(hModule, pOrigThunk->u1.AddressOfData));

    // Check for a match
    if (stricmp(szImportProc, (char*)(pByName->Name)) == 0) {
      // Save the original address
      FARPROC pOldProc = (FARPROC)(pRealThunk->u1.Function);

      // Don't hook a NULL pointer
      if (pOldProc == NULL)
        continue;

      // I found it.  Now I need to change the protection to
      //  writable before I do the blast.  Note that I am now
      //  blasting into the real thunk area!
      MEMORY_BASIC_INFORMATION mbi_thunk;

      VERIFY(VirtualQuery(
        pRealThunk, &mbi_thunk, sizeof(mbi_thunk)) >= sizeof(mbi_thunk));

      VERIFY(VirtualProtect(
        mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect));

      // Perform the hook
      pRealThunk->u1.Function = (PDWORD)pNewProc;

      DWORD dwOldProtect;

      // Change the protection back to what it was before I
      //  blasted.
      VERIFY(VirtualProtect(
        mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect));

      ASSERT(dwOldProtect == PAGE_READWRITE);

      SetLastError(ERROR_SUCCESS) ;
      return pOldProc;
    }
  }

  return NULL;
}
