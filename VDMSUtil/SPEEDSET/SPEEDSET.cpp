// SPEEDSET.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SPEEDSET.h"

UINT timerId;

BOOL APIENTRY DllMain(
  HANDLE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved )
{
  switch (ul_reason_for_call)	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
  }

  return TRUE;
}

//
// Periodicaly generates IRQs
//
void CALLBACK TimeProc(
  UINT uID,
  UINT uMsg,
  DWORD dwUser,
  DWORD dw1,
  DWORD dw2 )
{
  VDDSimulateInterrupt(IRQ_CTL, IRQ_LINE, 1);
}

//
// Initialization routine; called right after NTVDM.EXE loads VDDLoader.DLL
//   in its memory space
//
STDAPI_(void) VddInitialize(void) {
  timerId = 0;
  setCF(0);
}

//
// Dispatch routine; allows DOS applications to send commands to the 32-bit
//   environemnt.
// Parameters:
//   BX    = Command ID
//   DS:SI = Pointer to user-data buffer
//   DX    = User-data buffer length
STDAPI_(void) VddDispatch(void) {
  WORD dispCmd     = getBX();

  int retVal;

  switch (dispCmd) {
    case CMD_VDD_INIT:
      timerId = timeSetEvent(IRQ_INTERVAL, IRQ_INTERVAL/10, TimeProc, NULL, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
      if (timerId != 0) {
        retVal = 0;
      } else {
        retVal = -1;
      }
      break;

    case CMD_VDD_DESTROY:
      if (timerId != 0) {
        timeKillEvent(timerId);
      }
      retVal = 0;
      break;

    default:
      retVal = -1;
      break;
  }

  setAX(retVal);
}
