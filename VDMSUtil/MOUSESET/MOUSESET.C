#pragma option -mt -k- -p- -r- -G- -N-

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>

/* The interrupt */
#define INTR 0x33

struct INTR_REGS {
  unsigned bp, di, si, ds, es, dx, cx, bx, ax, ip, cs, fl;
};

unsigned int scalex, scaley;

void interrupt (*oldhandler)(void);

void interrupt handler(struct INTR_REGS r) {
  switch (r.ax) {
    case 3: /* INT 33,3 - Get Mouse Position and Button Status */
    case 5: /* INT 33,5 - Get Mouse Button Press Information */
    case 6: /* INT 33,6 - Get Mouse Button Release Information */
      asm pop bp
      asm pop di
      asm pop si
      asm pop ds
      asm pop es
      asm pop dx
      asm pop cx
      asm pop bx
      asm pop ax
      asm pushf
      asm call cs:oldhandler
      asm push ax
      asm push dx

      asm mov ax,cs:scalex
      asm mul cx
      asm shr ax,5
      asm shl dx,3
      asm or  ax,dx
      asm mov cx,ax

      asm pop dx

      asm mov ax,cs:scaley
      asm mul dx
      asm shr ax,5
      asm shl dx,3
      asm or  dx,ax

      asm pop ax
      asm iret
  }

  /* call through */
  asm pop bp
  asm pop di
  asm pop si
  asm pop ds
  asm pop es
  asm pop dx
  asm pop cx
  asm pop bx
  asm pop ax

  /* transfer control to the old interrupt handler */
  asm jmp cs:oldhandler
}

int main(int argc, char** argv) {
  char* endptr;
  long param;

  printf("Poor man's MOUSE hack for VESA (ver. 0.1)\nCopyright (C) 2001 Vlad ROMASCANU\n\n");

  if (argc < 4) {
    printf("Incorrect number of arguments\n  USAGE: MOUSESET xmax ymax program.exe\nwhere xmax would typically be 640 and ymax 480 (for 640x480)\n");
    return 1;
  }

  param = strtol(argv[1], &endptr, 10);

  if ((*endptr != '\0') || (param < 640) || (param > 2048)) {
    printf("'%s' is an invalid value.  Please enter a value between 640 and 2048.\n", argv[1]);
    return 2;
  }

  scalex = (param * 32) / 640;

  param = strtol(argv[2], &endptr, 10);

  if ((*endptr != '\0') || (param < 200) || (param > 2048)) {
    printf("'%s' is an invalid value.  Please enter a value between 200 and 2048.\n", argv[2]);
    return 2;
  }

  scaley = (param * 32) / 200;

  /* get the address of the current interrupt */
  oldhandler = getvect(INTR);

  /* install the new interrupt handler */
  setvect(INTR, handler);

  spawnvp(P_WAIT, argv[3], &(argv[3]));

  setvect(INTR, oldhandler);

  return 0;
}
