// SPEEDSET.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define IRQ_CTL       0     /* 0 = master, 1 = slave */

/////////////////////////////////////////////////////////////////////////////

enum VDDCommands {
  CMD_VDD_GETERROR  = 0x00,
  CMD_VDD_CFG_PUT   = 0x01,
  CMD_VDD_CFG_GET   = 0x02,
  CMD_VDD_START     = 0x03,
  CMD_VDD_SLEEP     = 0x04
};

#pragma pack (push, 1)

struct vdd_config_t {
  BYTE cMode;
  BYTE cIRQ;
  DWORD dwParam;
};

#pragma pack (pop)

/////////////////////////////////////////////////////////////////////////////

UINT hTimer;
UINT uPeriod;
TIMECAPS tCaps;
UINT refCount = 0;

CHAR szLastError[1024];
vdd_config_t config;

LPCSTR szUsage = "Usage: SPEEDSET speed\r\n ...where speed is a number between 100.0 (fastest) and -100.0 (slowest).\r\n";

UINT ParseCommandLine(LPCSTR, UINT);

/////////////////////////////////////////////////////////////////////////////

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
  VDDSimulateInterrupt(IRQ_CTL, config.cIRQ, 1);
}

//
// Initialization routine; called right after NTVDM.EXE loads VDDLoader.DLL
//   in its memory space
//
STDAPI_(void) VddInitialize(void) {
  hTimer = 0;
  uPeriod = 1;

  if (timeGetDevCaps(&tCaps, sizeof(tCaps)) == TIMERR_NOERROR) {
    tCaps.wPeriodMin = 1;
    tCaps.wPeriodMax = 1000;
  }

  szLastError[0] = '\0';

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
  UINT retVal      = 0;
  WORD dispCmd     = getBX();
  WORD paramSeg    = getDS();
  WORD paramOffset = getSI();
  WORD paramLen    = getDX();

  LPVOID param = GetVDMPointer(MAKELONG(paramOffset, paramSeg), paramLen, FALSE);

  switch (dispCmd) {
    case CMD_VDD_GETERROR:
      if (paramLen > 0) {
        LPTSTR szParam = (LPTSTR)param;
        strncpy(szParam, szLastError, paramLen);
        szParam[paramLen - 1] = '\0';
      } else {
        strncpy(szLastError, "Unexpected error\r\n", sizeof(szLastError));
        retVal = 1;
      }
      break;

    case CMD_VDD_CFG_PUT:
      retVal = ParseCommandLine((LPCSTR)param, paramLen);
      break;

    case CMD_VDD_CFG_GET:
      if (++refCount > 1) {
        refCount--;
        strncpy(szLastError, "Another instance of SPEEDSET appears to be already running.\r\n", sizeof(szLastError));
        retVal = 1;
      } else {
        _ASSERTE(paramLen == 6);
        ((vdd_config_t*)param)->cMode   = config.cMode;
        ((vdd_config_t*)param)->cIRQ    = config.cIRQ;
        ((vdd_config_t*)param)->dwParam = config.dwParam;
      }
      break;

    case CMD_VDD_START:
      hTimer = timeSetEvent(uPeriod, uPeriod / 10, TimeProc, NULL, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
      if (hTimer != 0) {
        retVal = 0;
      } else {
        strncpy(szLastError, "Unable to create Windows timer.\r\n", sizeof(szLastError));
        retVal = 1;
      }
      break;

    case CMD_VDD_SLEEP:
      Sleep(*((WORD*)param));
      break;

    default:
      strncpy(szLastError, "Invalid command -- check for version mismatch between .EXE and .DLL.\r\n", sizeof(szLastError));
      retVal = 1;
      break;
  }

  FreeVDMPointer(MAKELONG(paramOffset, paramSeg), paramLen, param, FALSE);

  setAX(retVal);
}

UINT ParseCommandLine(LPCSTR cmd, UINT cmdLen) {
  CHAR buf[128];

  _ASSERTE(cmdLen <= sizeof(buf));

  cmdLen = min(cmdLen, sizeof(buf) - 1);
  OemToCharBuff(cmd, buf, cmdLen);
  buf[cmdLen] = '\0';

  double speedRate = 0;
  int IRQ = 0;

  switch (sscanf(buf, "%lf IRQ%d", &speedRate, &IRQ)) {
    case 1:
      config.cIRQ = 6;  // default
      break;
    case 2:
      config.cIRQ = ((IRQ >= 0) && (IRQ < 8)) ? IRQ : 6;
      break;
    default:
      strncpy(szLastError, szUsage, sizeof(szLastError));
      return 1;
  }

  if ((speedRate > 100.0) || (speedRate < -100.0)) {
    strncpy(szLastError, "Invalid value specified for speed.\r\nPlease specify a value between 100.0 and -100.0\r\n", sizeof(szLastError));
    strncat(szLastError, szUsage, sizeof(szLastError) - strlen(szLastError));
    return 2;
  }

  if (speedRate > 0.0) {
    // Assume "speedRate" value represents number of (extra) interrupts in any given ms,
    // i.e. speedRate was given for uPeriod = 1ms; for the least amount of jitter (sampling
    // noise) we want to bring samplerate as close to 1.0 as possible (normalize it) while
    // contrained by the fact that uPeriod is an integer and has to fit in the timer's
    // capabilities (min/max period)  Below we normalize speedRate and compute the ideal
    // period.
    double idealPeriod = 1.0 / speedRate;
    uPeriod = max(tCaps.wPeriodMin, min(tCaps.wPeriodMax, (UINT)idealPeriod));
    speedRate = speedRate * uPeriod;

    _ASSERTE(speedRate > 0.0);
    _ASSERTE(speedRate < 65536.0);

    config.cMode = 0;
    config.dwParam = (DWORD)(speedRate * 65536.0);
  } else {
    // To slow down effectively we would like to fragment the DOS program's time as much
    // as possible.  A best effor would result in teh DOS program running uninterrupted
    // in slices of 1ms or less (that's the resolution of our timer and Sleep() functions,
    // i.e. 1 millisecond, so we can't do better than that).  If we liek a 90% slowdown
    // then 90% of any given period of time has to be spent sleeping and 10% doing actual
    // work in DOS.  If that 10% is 1ms (minimum length slice) then we have to sleep for
    // 9ms.  The timer period and sleep slice length is computed below.
    double overhead = speedRate / -100.0;
    uPeriod = max(tCaps.wPeriodMin, min(tCaps.wPeriodMax, (UINT)(1/(1.0 - overhead))));
    speedRate = overhead * uPeriod;

    _ASSERTE(speedRate > 0.0);
    _ASSERTE(speedRate < 65536.0);

    config.cMode = 1;
    config.dwParam = (DWORD)(speedRate * 65536.0);
  }

  return 0;
}
