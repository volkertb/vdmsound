#pragma option -mt -k- -p- -r- -G- -N-

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>

/* The interrupt */
#define INTR 0x28

#ifdef __cplusplus
    #define __CPPARGS ...
    #define max(a,b) (((a) > (b)) ? (a) : (b))
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#else
    #define __CPPARGS
#endif

unsigned int PITCount;

void interrupt ( *oldhandler)(__CPPARGS);

/* reduce heaplength and stacklength to make a smaller program in memory */
extern unsigned _heaplen = 4096;
extern unsigned _stklen  = 512;

void interrupt handler(__CPPARGS) {
  static unsigned long lastTime = 0;
  unsigned long thisTime;

  /* reprogram the PIT every now and then */

  thisTime = *((unsigned long far*)(MK_FP(0x40,0x6c)));

  if (thisTime - lastTime > min(1000000l/PITCount, 1000)) {
    lastTime = thisTime;
    asm {
      cli
      mov al,36h
      out 43h,al
      mov cx,PITCount
      mov al,cl
      out 40h,al
      mov al,ch
      out 40h,al
      sti
    }
  }

  /* call the old interrupt handler */
  oldhandler();
}

int main(int argc, char** argv) {
  printf("Poor man's PIT reprogrammer (ver. 0.2)\nCopyright (C) 2001 Vlad ROMASCANU\n\n");

  if (argc < 3) {
    printf("Incorrect number of arguments\n  USAGE: PITSET value program.exe\nwhere value lies between 0 (fastest) and 100 (slowest)\n");
    return 1;
  }

  int percent = 100;

  if ((sscanf(argv[1], "%d", &percent) == 0) || (percent < 0) || (percent > 100)) {
    printf("'%s' is an invalid value.  Please enter a value between 0 and 100.\n", argv[1]);
    return 2;
  }

  PITCount = min(max(65536lu * percent / 100, 1), 65535);

  printf("Reprogramming PIT interval: %d%% (countdown value = 0x%04x)\n", percent, PITCount);

  /* get the address of the current interrupt */
  oldhandler = getvect(INTR);

  /* install the new interrupt handler */
  setvect(INTR, handler);

  spawnvp(P_WAIT, argv[2], &(argv[2]));

  setvect(INTR, oldhandler);

  return 0;
}
