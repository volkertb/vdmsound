#pragma option -mt -O1 -k- -p- -r- -G- -N- -1 -f-

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
#include <stdlib.h>
#include <time.h>
#include <process.h>

#include <assert.h>

// Define common stuff between the VDD DLL and the DOS loader
#include "speedset.h"

/* PIC base-port for IRQ */
#define IRQ_PORT (IRQ_CTL == 0 ? 0x20 : 0xa0)

/* IRQ line for IRQ */
#define IRQ_LINE_MASK_ON (1 << (IRQ_LINE))
#define IRQ_LINE_MASK_OFF (~(1 << (IRQ_LINE)))

/* Interrupt for IRQ */
#define IRQ_INTR ((IRQ_CTL == 0 ? 0x08 : 0x70) + IRQ_LINE)

/* Various Borland C++ specific stuff */
#define _SEG(p) FP_SEG(p)
#define _stricmp stricmp
#define _strlwr strlwr
#define __asm asm

#ifdef __cplusplus
    #define __CPPARGS ...
    #define max(a,b) (((a) > (b)) ? (a) : (b))
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#else
    #define __CPPARGS
#endif

static const unsigned int coeffs[101] = {
      0,    25,    51,    79,   108,   139,   171,   204,   240,   277,
    316,   357,   400,   445,   492,   542,   594,   649,   706,   767,
    830,   897,   967,  1040,  1117,  1198,  1283,  1373,  1466,  1565,
   1668,  1777,  1891,  2010,  2136,  2268,  2406,  2552,  2704,  2865,
   3033,  3210,  3396,  3590,  3795,  4010,  4236,  4472,  4721,  4982,
   5257,  5544,  5847,  6164,  6498,  6848,  7215,  7601,  8006,  8431,
   8878,  9347,  9840, 10357, 10900, 11470, 12068, 12697, 13357, 14050,
  14777, 15541, 16343, 17186, 18070, 18999, 19974, 20998, 22073, 23201,
  24387, 25631, 26938, 28310, 29750, 31263, 32851, 34519, 36270, 38108,
  40039, 42066, 44194, 46429, 48776, 51240, 53827, 56543, 59395, 62390, 65535
};

//////////////////////////////////////////////////////////////////////////////



int rate; /* slow down/speed up rate */
int slow; /* whether to slow down or speed up */

void interrupt ( *oldhandler)(__CPPARGS);

/* reduce heaplength and stacklength to make a smaller program in memory */
extern unsigned _heaplen = 4096;
extern unsigned _stklen  = 512;

void interrupt handler(__CPPARGS) {
  int i;

  if (slow) {
    delay(rate);
  } else {
    for (i = 0; i < rate; i++) {
      __asm int 8;
    }
  }

  /* specific EOI */
  outportb(IRQ_PORT, 0x60 | IRQ_LINE);
}



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



int main(int argc, char** argv) {
  int percent = 0;
  int retVal  = 0;
  HANDLE hVDD;

  printf("Poor man's DOS speed controller (ver. 0.4)\nCopyright (C) 2001 Vlad ROMASCANU\n\n");

  if (argc < 3) {
    printf("Incorrect number of arguments\n  USAGE: SPEEDSET value program.exe\nwhere value lies between -100 (fastest) and 100 (slowest)\n");
    return 1;
  }

  if ((sscanf(argv[1], "%d", &percent) == 0) || (percent < -100) || (percent > 100)) {
    printf("'%s' is an invalid value.  Please enter a value between -100 and 100.\n", argv[1]);
    return 2;
  }

  if (percent < 0) {
    rate = (unsigned int)((coeffs[-percent] * 100l * IRQ_INTERVAL) / 65535l);
    slow = 0;
  } else {
    rate = (unsigned int)((coeffs[percent] * (long)IRQ_INTERVAL) / 65535l);
    slow = 1;
  }

  /* mask IRQ */
  outportb(IRQ_PORT + 1, inportb(IRQ_PORT + 1) | IRQ_LINE_MASK_ON);

  /* get the address of the current interrupt */
  oldhandler = getvect(IRQ_INTR);

  /* install the new interrupt handler */
  setvect(IRQ_INTR, handler);

  /* unmask IRQ */
  outportb(IRQ_PORT + 1, inportb(IRQ_PORT + 1) & IRQ_LINE_MASK_OFF);

  delay(500);   // let windows update its console

  if ((strlen(argv[0]) > 4) && (argv[0][strlen(argv[0]) - 4] == '.')) {
    argv[0][strlen(argv[0]) - 3] = 'D';
    argv[0][strlen(argv[0]) - 2] = 'L';
    argv[0][strlen(argv[0]) - 1] = 'L';
  }

  if (!RegisterModule(argv[0], "VddInitialize", "VddDispatch", &hVDD)) {
    printf("Unable to load the VDD DLL (%s).\n", argv[0]);
    retVal = 3;
  } else {
    if (DispatchCall(hVDD, CMD_VDD_INIT, NULL, 0) != 0) {
      printf("Unable to initialize the VDD DLL (%s).\n", argv[0]);
      retVal = 3;
    } else {
      spawnvp(P_WAIT, argv[2], &(argv[2]));

      if (DispatchCall(hVDD, CMD_VDD_DESTROY, NULL, 0) != 0) {
        printf("Unable to release the VDD DLL (%s).\n", argv[0]);
        retVal = 4;
      }
    }

    UnRegisterModule(hVDD);
  }

  /* mask IRQ */
  outportb(IRQ_PORT + 1, inportb(IRQ_PORT + 1) | IRQ_LINE_MASK_ON);

  setvect(IRQ_INTR, oldhandler);

  return retVal;
}
