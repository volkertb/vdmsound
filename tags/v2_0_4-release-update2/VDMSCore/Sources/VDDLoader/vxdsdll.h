#ifndef __VXDSDLL_H
#define __VXDSDLL_H

#ifdef __cplusplus
extern "C" {
#endif

/* constants */

#define VDD_DMA_ADDR    0x01
#define VDD_DMA_COUNT   0x02
#define VDD_DMA_PAGE    0x04
#define VDD_DMA_STATUS  0x08
#define VDD_DMA_ALL \
          ( VDD_DMA_ADDR | VDD_DMA_COUNT | VDD_DMA_PAGE | VDD_DMA_STATUS )

#define ICA_MASTER 0
#define ICA_SLAVE  1

typedef enum _VDD_REGISTER {
    VDD_REG_EAX	= 0,
  	VDD_REG_AX	= 1,
  	VDD_REG_AL	= 2,
  	VDD_REG_AH	= 3,
  	VDD_REG_EBX	= 4,
  	VDD_REG_BX	= 5,
  	VDD_REG_BL	= 6,
  	VDD_REG_BH	= 7,
  	VDD_REG_ECX	= 8,
  	VDD_REG_CX	= 9,
  	VDD_REG_CL	= 10,
  	VDD_REG_CH	= 11,
  	VDD_REG_EDX	= 12,
  	VDD_REG_DX	= 13,
  	VDD_REG_DL	= 14,
  	VDD_REG_DH	= 15,
  	VDD_REG_SS	= 16,
  	VDD_REG_ESP	= 17,
  	VDD_REG_SP	= 18,
  	VDD_REG_EBP	= 19,
  	VDD_REG_BP	= 20,
  	VDD_REG_DS	= 21,
  	VDD_REG_ESI	= 22,
  	VDD_REG_SI	= 23,
  	VDD_REG_ES	= 24,
  	VDD_REG_EDI	= 25,
  	VDD_REG_DI	= 26,
  	VDD_REG_CS	= 27,
  	VDD_REG_EIP	= 28,
  	VDD_REG_IP	= 29
}	VDD_REGISTER;

typedef enum _VDD_FLAG {
    VDD_FLAG_CARRY	= 0,
	  VDD_FLAG_PARITY	= 1,
	  VDD_FLAG_AUX	  = 2,
	  VDD_FLAG_ZERO	  = 3,
	  VDD_FLAG_SIGN	  = 4,
	  VDD_FLAG_TRAP	  = 5,
	  VDD_FLAG_INT	  = 6,
	  VDD_FLAG_DIR	  = 7,
	  VDD_FLAG_OFLOW	= 8
}	VDD_FLAG;

/* callbacks */

typedef VOID (*PFNVDD_INB)        (WORD iport, BYTE* data);
typedef VOID (*PFNVDD_INW)        (WORD iport, WORD* data);
typedef VOID (*PFNVDD_INSB)       (WORD iport, BYTE* data,WORD count);
typedef VOID (*PFNVDD_INSW)       (WORD iport, WORD* data,WORD count);
typedef VOID (*PFNVDD_OUTB)       (WORD iport, BYTE data);
typedef VOID (*PFNVDD_OUTW)       (WORD iport, WORD data);
typedef VOID (*PFNVDD_OUTSB)      (WORD iport, BYTE* data,WORD count);
typedef VOID (*PFNVDD_OUTSW)      (WORD iport, WORD* data,WORD count);

typedef VOID (*PFNVDD_UCREATE)    (USHORT DosPDB);
typedef VOID (*PFNVDD_UTERMINATE) (USHORT DosPDB);
typedef VOID (*PFNVDD_UBLOCK)     (VOID);
typedef VOID (*PFNVDD_URESUME)    (VOID);

typedef VOID (*PFNVDD_DMACALLB)   (DWORD);

/* structures */

#pragma pack (push, 1)

typedef struct _VDD_IO_HANDLERS {
    PFNVDD_INB    inb_handler;
    PFNVDD_INW    inw_handler;
    PFNVDD_INSB   insb_handler;
    PFNVDD_INSW   insw_handler;
    PFNVDD_OUTB   outb_handler;
    PFNVDD_OUTW   outw_handler;
    PFNVDD_OUTSB  outsb_handler;
    PFNVDD_OUTSW  outsw_handler;
} VDD_IO_HANDLERS, *PVDD_IO_HANDLERS;

typedef struct _VDD_IO_PORTRANGE {
    WORD  First;
    WORD  Last;
} VDD_IO_PORTRANGE, *PVDD_IO_PORTRANGE;

typedef struct _VDD_DMA_INFO {
    WORD  addr;
    WORD  count;
    WORD  page;
    BYTE  status;
    BYTE  mode;
    BYTE  mask;
} VDD_DMA_INFO, *PVDD_DMA_INFO;

#pragma pack (pop)

/* exported functions */

PBYTE
WINAPI
MGetVdmPointer(
    ULONG Address,
    ULONG Size,
    UCHAR ProtectedMode
    );

VOID
WINAPI
VDDDeInstallIOHook(
    HANDLE hVDD,
    WORD cPortRange,
    PVDD_IO_PORTRANGE pPortRange
    );

BOOL
WINAPI
VDDDeInstallUserHook(
    HANDLE hVDD
    );

INT
WINAPI
VDDHookDMA(
    WORD iChannel,
    PFNVDD_DMACALLB callback
    );

INT
WINAPI
VDDHookIRQ(
    BYTE line
    );

BOOL
WINAPI
VDDInstallIOHook(
    HANDLE hVDD,
    WORD cPortRange,
    PVDD_IO_PORTRANGE pPortRange,
    PVDD_IO_HANDLERS IOhandler
    );

BOOL
WINAPI
VDDInstallUserHook(
    HANDLE hVDD,
    PFNVDD_UCREATE Ucr_Handler,
    PFNVDD_UTERMINATE Uterm_Handler,
    PFNVDD_UBLOCK Ublock_handler,
    PFNVDD_URESUME Uresume_handler
    );

BOOL
WINAPI
VDDQueryDMA(
    HANDLE hVDD,
    WORD iChannel,
    PVDD_DMA_INFO pDmaInfo
    );

DWORD
WINAPI
VDDRequestDMA(
    HANDLE hVDD,
    WORD iChannel,
    PVOID Buffer,
    DWORD length
    );

BOOL
WINAPI
VDDSetDMA(
    HANDLE hVDD,
    WORD iChannel,
    WORD fDMA,
    PVDD_DMA_INFO Buffer
    );

VOID
WINAPI
VDDSimulateInterrupt(
    INT ms,
    BYTE line,
    INT count
    );

VOID
WINAPI
VDDTerminateVDM(
    );

VOID
WINAPI
VDDUnhookDMA(
    WORD iChannel
    );

VOID
WINAPI
VDDUnhookIRQ(
    BYTE line
    );

DWORD
WINAPI
vGetRegister(
    VDD_REGISTER reg
    );

VOID
WINAPI
vSetRegister(
    VDD_REGISTER reg,
    DWORD regVal
    );

BOOL
WINAPI
vGetFlag(
    VDD_FLAG flag
    );

VOID
WINAPI
vSetFlag(
    VDD_FLAG flag,
    BOOL flagVal
    );

#ifdef __cplusplus
}
#endif

#define	getEAX()	((ULONG) vGetRegister(VDD_REG_EAX))
#define	getAX() 	((USHORT)vGetRegister(VDD_REG_AX))
#define	getAL() 	((BYTE)  vGetRegister(VDD_REG_AL))
#define getAH()  ((BYTE)  vGetRegister(VDD_REG_AH))
#define	getEBX()	((ULONG) vGetRegister(VDD_REG_EBX))
#define	getBX() 	((USHORT)vGetRegister(VDD_REG_BX))
#define	getBL() 	((BYTE)  vGetRegister(VDD_REG_BL))
#define	getBH() 	((BYTE)  vGetRegister(VDD_REG_BH))
#define	getECX()	((ULONG) vGetRegister(VDD_REG_ECX))
#define	getCX() 	((USHORT)vGetRegister(VDD_REG_CX))
#define	getCL() 	((BYTE)  vGetRegister(VDD_REG_CL))
#define	getCH() 	((BYTE)  vGetRegister(VDD_REG_CH))
#define	getEDX()	((ULONG) vGetRegister(VDD_REG_EDX))
#define	getDX() 	((USHORT)vGetRegister(VDD_REG_DX))
#define	getDL() 	((BYTE)  vGetRegister(VDD_REG_DL))
#define	getDH() 	((BYTE)  vGetRegister(VDD_REG_DH))
#define	getESP()	((ULONG) vGetRegister(VDD_REG_ESP))
#define	getSP() 	((USHORT)vGetRegister(VDD_REG_SP))
#define	getEBP()	((ULONG) vGetRegister(VDD_REG_EBP))
#define	getBP() 	((USHORT)vGetRegister(VDD_REG_BP))
#define	getESI()	((ULONG) vGetRegister(VDD_REG_ESI))
#define	getSI() 	((USHORT)vGetRegister(VDD_REG_SI))
#define	getEDI()	((ULONG) vGetRegister(VDD_REG_EDI))
#define	getDI() 	((USHORT)vGetRegister(VDD_REG_DI))
#define	getEIP()	((ULONG) vGetRegister(VDD_REG_EIP))
#define	getIP() 	((USHORT)vGetRegister(VDD_REG_IP))
#define	getCS() 	((USHORT)vGetRegister(VDD_REG_CS))
#define	getSS() 	((USHORT)vGetRegister(VDD_REG_SS))
#define	getDS() 	((USHORT)vGetRegister(VDD_REG_DS))
#define	getES() 	((USHORT)vGetRegister(VDD_REG_ES))

#define	getCF() 	((BOOL)vGetFlag(VDD_FLAG_CARRY))
#define	getPF() 	((BOOL)vGetFlag(VDD_FLAG_PARITY))
#define	getAF() 	((BOOL)vGetFlag(VDD_FLAG_AUX))
#define	getZF() 	((BOOL)vGetFlag(VDD_FLAG_ZERO))
#define	getSF() 	((BOOL)vGetFlag(VDD_FLAG_SIGN))
#define	getIF() 	((BOOL)vGetFlag(VDD_FLAG_INT))
#define	getDF() 	((BOOL)vGetFlag(VDD_FLAG_DIR))
#define	getOF() 	((BOOL)vGetFlag(VDD_FLAG_OFLOW))

#define	setEAX(value)	vSetRegister(VDD_REG_EAX, (ULONG) value)
#define	setAX(value)	vSetRegister(VDD_REG_AX,  (USHORT)value)
#define	setAH(value)	vSetRegister(VDD_REG_AH,  (BYTE)  value)
#define	setAL(value)	vSetRegister(VDD_REG_AL,  (BYTE)  value)
#define	setEBX(value)	vSetRegister(VDD_REG_EBX, (ULONG) value)
#define	setBX(value)	vSetRegister(VDD_REG_BX,  (USHORT)value)
#define	setBH(value)	vSetRegister(VDD_REG_BH,  (BYTE)  value)
#define	setBL(value)	vSetRegister(VDD_REG_BL,  (BYTE)  value)
#define	setECX(value)	vSetRegister(VDD_REG_ECX, (ULONG) value)
#define	setCX(value)	vSetRegister(VDD_REG_CX,  (USHORT)value)
#define	setCH(value)	vSetRegister(VDD_REG_CH,  (BYTE)  value)
#define	setCL(value)	vSetRegister(VDD_REG_CL,  (BYTE)  value)
#define	setEDX(value)	vSetRegister(VDD_REG_EDX, (ULONG) value)
#define	setDX(value)	vSetRegister(VDD_REG_DX,  (USHORT)value)
#define	setDH(value)	vSetRegister(VDD_REG_DH,  (BYTE)  value)
#define	setDL(value)	vSetRegister(VDD_REG_DL,  (BYTE)  value)
#define	setESP(value)	vSetRegister(VDD_REG_ESP, (ULONG) value)
#define	setSP(value)	vSetRegister(VDD_REG_SP,  (USHORT)value)
#define	setEBP(value)	vSetRegister(VDD_REG_EBP, (ULONG) value)
#define	setBP(value)	vSetRegister(VDD_REG_BP,  (USHORT)value)
#define	setESI(value)	vSetRegister(VDD_REG_ESI, (ULONG) value)
#define	setSI(value)	vSetRegister(VDD_REG_SI,  (USHORT)value)
#define	setEDI(value)	vSetRegister(VDD_REG_EDI, (ULONG) value)
#define	setDI(value)	vSetRegister(VDD_REG_DI,  (USHORT)value)
#define	setEIP(value)	vSetRegister(VDD_REG_EIP, (ULONG) value)
#define	setIP(value)	vSetRegister(VDD_REG_IP,  (USHORT)value)
#define	setCS(value)	vSetRegister(VDD_REG_CS,  (USHORT)value)
#define	setSS(value)	vSetRegister(VDD_REG_SS,  (USHORT)value)
#define	setDS(value)	vSetRegister(VDD_REG_DS,  (USHORT)value)
#define	setES(value)	vSetRegister(VDD_REG_ES,  (USHORT)value)

#define	setCF(value)	vSetFlag(VDD_FLAG_CARRY,  value != 0)
#define	setPF(value)	vSetFlag(VDD_FLAG_PARITY, value != 0)
#define	setAF(value)	vSetFlag(VDD_FLAG_AUX,    value != 0)
#define	setZF(value)	vSetFlag(VDD_FLAG_ZERO,   value != 0)
#define	setSF(value)	vSetFlag(VDD_FLAG_SIGN,   value != 0)
#define	setIF(value)	vSetFlag(VDD_FLAG_INT,    value != 0)
#define	setDF(value)	vSetFlag(VDD_FLAG_DIR,    value != 0)
#define	setOF(value)	vSetFlag(VDD_FLAG_OFLOW,  value != 0)

#define GetVDMPointer(Address, Size, Mode) \
          MGetVdmPointer(Address, Size, Mode)

#define FreeVDMPointer(Address, Size, Buffer, Mode) \
          TRUE

#define VDDSimulateInterrupt(ms, line, count) \
          VDDSimulateInterrupt(0, (ms) * 8 + (line), count)

#endif //__VXDSDLL_H
