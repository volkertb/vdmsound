#ifndef __DOSDRV_H_
#define __DOSDRV_H_

#define IRQ_INTERVAL  10    /* interval, in ms */
#define IRQ_LINE      6     /* irq line */
#define IRQ_CTL       0     /* 0 = master, 1 = slave */

enum VDDCommands {
  CMD_VDD_INIT    = 0x00,
  CMD_VDD_DESTROY = 0x10
};

#endif //__DOSDRV_H_
