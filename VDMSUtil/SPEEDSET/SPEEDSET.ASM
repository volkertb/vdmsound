		.286
		model TINY

; ---------------------------------------------------------------------------

OFF_PSP_ENV	equ	2ch
OFF_PSP_CMD_LEN	equ	080h
OFF_PSP_CMD	equ	081h
CMD_VDD_ERR_GET	equ	0
CMD_VDD_CFG_PUT	equ	1
CMD_VDD_CFG_GET	equ	2
CMD_VDD_START  	equ	3
CMD_VDD_SLEEP	equ	4

; ---------------------------------------------------------------------------

TEXT		segment byte public 'CODE' use16
		assume cs:TEXT
		org 100h

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


		public start
start		proc near

		; Set stack
		mov	sp, offset stack_bottom

		; Save PSP
		mov	TEXT:[wPSP], ds

		; Print startup banner
		push	cs
		push	offset szAbout
		call	PrintZ

		; Locate the executable's name in the environment
		mov	ax, ds:[OFF_PSP_ENV]    ; Pointer to environment block, in the PSP
		mov	es, ax
		xor	ax, ax
		mov	di, ax
		cld

;
;	Scan the end of the environment to obtain this .exe's path, which we
;	need in order to know where the VDD .dll file is
;
start_scanenv:
		; Find SPEEDSET.EXE's full path at the top of the environment
		mov	cx, 7FFFh
		sub	cx, di                  ; Environment is at most 32k long
		repne	scasb                   ; Look for '\0'
		jcxz	start_scanenv_error     ; Unexpected end of the environment
		cmp	byte ptr es:[di], al    ; Second null in a row?
		jne	start_scanenv           ; Keep on looking until we hit the end of the environment
		inc	di
		inc	al
		cmp	word ptr es:[di], ax	; dw 1 ?
		jne	start_scanenv_error     ; Does not really look like the end of the environment, bail out
		inc	di
		inc	di
		mov	dx, di			; Save in dx the offset of the executable name (which comes right after the environment)

		; Modify the SPEEDSET.EXE path's extension to '.dll' to make
		; it point to the VDD
		xor	al, al                  ; Look for the end of the executable name (ASCIIZ)
		mov	cx, 7FFFh
		sub	cx, di
		repne	scasb                   ; Look for '\0' (again)
		jcxz	start_scanenv_error
		mov	cx, di
		sub	cx, dx			; How long is the executabel name?
		cmp	dx, 5
		jbe	start_scanenv_error     ; Bail out if executable name is less than 5 characters (X.EXE) long (excluding the trailing '\0')
		cmp	byte ptr es:[di-5], '.'
		jne	start_scanenv_error     ; Bail out if there isn't a three-character extension (e.g. ".exe")
		mov	byte ptr es:[di-4], 'd'
		mov	byte ptr es:[di-3], 'l'
		mov	byte ptr es:[di-2], 'l' ; replace '.exe' with '.dll'

		mov	di, dx
		jmp	start_load_vdd

start_scanenv_error:
		; Try default SPEEDSET VDD dll name
		mov	di, TEXT
		mov	es, di
		mov	di, offset szVDD

;
;	Load and configure the VDD
;
start_load_vdd:
		; Load the SPEEDSET VDD into NTVDM's memory space
		push	es
		push	di
		call	RegisterModule
		or	al, al
		jnz	start_setup_vdd

		; Print error
		push	cs
		push	offset szVDDError1
		call	PrintZ
		push	es
		push	di
		call	PrintZ
		push	cs
		push	offset szVDDError2
		call	PrintZ
		jmp	start_exit

start_setup_vdd:
		; Pass the SPEEDSET's command line to the VDD for parsing
		mov	al, byte ptr ds:[OFF_PSP_CMD_LEN]
		xor	ah, ah
		push	ax
		push	ds
		push	OFF_PSP_CMD
		push	CMD_VDD_CFG_PUT
		call	DispatchCall
		or	ax, ax
		jnz	start_setup_vdd_error

		; Retrieve interpretation of the command line from the VDD
		push	6
		push	cs
		push	offset vdd_cMode
		push	CMD_VDD_CFG_GET
		call	DispatchCall
		or	ax, ax
		jnz	start_setup_vdd_error

start_set_isr:
		; Set the interrupt service routine
		mov	cl, TEXT:[vdd_cIRQ]
		and	cl, 0Fh
		mov	bl, 1
		shl	bl, cl

		; Mask IRQ
		in	al, 21h
		or	al, bl
		out	21h, al

		; Install new IRQ handler
		mov	ax, 2508h
		add	al, cl
		push	cs
		pop	ds
		mov	dx, offset IRQ_Handler
		int	21h

		; Unmask IRQ
		in	al, 21h
		not	bl
		and	al, bl
		out	21h, al

		add	cl, '0'
		mov	TEXT:[szDone1], cl

		push	cs
		push	offset szDone
		call	PrintZ

		; Start generation of IRQs
		push	0
		push	0
		push	0
		push	CMD_VDD_START
		call	DispatchCall
		or	ax, ax
		jnz	start_setup_vdd_error

		; Free environment
		mov	ah, 49h
		mov	ds, TEXT:[wPSP];
		mov	es, ds:[OFF_PSP_ENV]
		int	21h

		; Terminate and stay resident
		mov	ax, 3100h
		mov	dx, offset stack_bottom
		add	dx, 0Fh
		shr	dx, 4
		int	21h

start_setup_vdd_error:
		; Print error from VDD
		mov	ax, sp
		sub	ax, offset stack_top
		sub	ax, 030h                ; slack
		push	ax
		mov	byte ptr TEXT:[stack_top], 0
		push	ss
		push	offset stack_top
		push	CMD_VDD_ERR_GET
		call	DispatchCall
		push	ss
		push	offset stack_top
		call	PrintZ

start_exit:
		mov	ax, 4C01h               ; DOS terminate program function
		int	21h                     ; terminate the program with code al=01h (error)
start		endp

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


IRQ_Handler	proc	far
		push	ax
		push	cx

		; Check against reentrance
		cmp	TEXT:[ISR_busy], 0
		jne	IRQ_Handler_exit

		; Increment counter and check for rollover from the fractional
		; into the integer part of the 16.16 fixed-point form
		xor	cx, cx
		mov	ax, TEXT:[vdd_wCounter]
		add	ax, word ptr TEXT:[vdd_dwParam]
		adc	cx, word ptr TEXT:[vdd_dwParam + 2]
		mov	TEXT:[vdd_wCounter], ax
		jcxz	IRQ_Handler_exit

		; Decide whether we have to speed up or slow down the system
		cmp	TEXT:[vdd_cMode], 0
		jnz	IRQ_Handler_slow

IRQ_Handler_fast:
		mov	TEXT:[ISR_busy], 1

IRQ_Handler_fast_fire:
		int	08h
		loopnz	IRQ_Handler_fast_fire
		jmp	IRQ_Handler_exit

IRQ_Handler_slow:
		; Perform a Sleep in Win32
		mov	TEXT:[vdd_wTemp], cx
		push	2
		push	cs
		push	offset vdd_wTemp
		push	CMD_VDD_SLEEP
		call	DispatchCall

IRQ_Handler_exit:
		mov	al, TEXT:[vdd_cIRQ]
		and	al, 0Fh
		or	al, 060h
		out	20h, al             ; Specific EOI

		pop	cx
		pop	ax

		mov	TEXT:[ISR_busy], 0
		iret
IRQ_Handler	endp

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


PrintZ		proc	near	lpszString:dword
		push	bp
		mov	bp, sp
		pushf
		push	bx
		push	cx
		push	dx
		push	es
		push	di
		push	ds

		cld
		xor	ax, ax
		mov	cx, 7FFh
		les	di, lpszString
		push	es
		pop	ds
		mov	dx, di
		repne	scasb
		jcxz	PrintZ_error

		mov	ah, 40h
		mov	bx, 0001h
		sub	di, dx
		mov	cx, di
		dec	cx
		int	21h
		xor	ax, ax
		jmp	PrintZ_exit

PrintZ_error:
		inc	ax

PrintZ_exit:
		pop	ds
		pop	di
		pop	es
		pop	dx
		pop	cx
		pop	bx
		popf
		pop	bp

		ret	4
PrintZ		endp

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


RegisterModule	proc	near	lpszVDD:dword,result:byte
		push	bp
		mov	bp, sp
		push	bx
		push	es
		push	di
		push	ds
		push	si

		lds	si, lpszVDD
		mov	di, offset szInit
		mov	bx, offset szDisp

		mov	ax, ds                  ; Perform a segment adjust for szInit and szDisp: NTVDM needs everything to be relative to the same segment
		mov	es, ax
		sub	ax, TEXT
		shl	ax, 4
		sub	di, ax
		sub	bx, ax

		db	0C4h
		db	0C4h
		db	58h
		db	0h

		mov	TEXT:[hVDD], ax
		cmc
		lahf
		and	ah, 1
		xchg	ah, al

		pop	si
		pop	ds
		pop	di
		pop	es
		pop	bx
		pop	bp

		ret	4
RegisterModule	endp

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


UnRegisterModule	proc	near
		push	bp
		mov	bp, sp
		push	ax
		mov	ax, TEXT:[hVDD]

		db	0C4h
		db	0C4h
		db	58h
		db	1h

		pop	ax
		pop	bp

		ret
UnRegisterModule	endp

; ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ S U B R O U T I N E ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦


DispatchCall	proc	near	uCmd:word,lpParam:dword,uParamLen:word,result:word
		push	bp
		mov	bp, sp
		push	bx
		push	dx
		push	ds
		push	si

		mov	ax, TEXT:[hVDD]
		mov	bx, uCmd
		lds	si, lpParam
		mov	dx, uParamLen

		db	0C4h
		db	0C4h
		db	58h
		db	2h

		pop	si
		pop	ds
		pop	dx
		pop	bx
		pop	bp

		ret	8
DispatchCall	endp

; ---------------------------------------------------------------------------

hVDD		dw	0
wPSP		dw	0

szAbout		db	'Poor man',27h,'s DOS speed controller (ver. 1.0.1)',0Dh,0Ah,'Copyright (C) 2001 Vlad ROMASCANU',0Dh,0Ah,0Ah,0
szDone		db	'SPEEDSET is now resident on IRQ '
szDone1		db	'?'
szDone2		db	'.',0Dh,0Ah,0

szVDD		db	'SpeedSet.dll',0
szInit		db	'VddInitialize',0
szDisp		db	'VddDispatch',0

szVDDError1	db	'Error: Unable to load ',27h,0
szVDDError2	db	27h,'.',0Dh,0Ah,0

vdd_cMode	db	0	; slowdown or acceleration mode
vdd_cIRQ	db	0	; IRQ through which the TSR and the VDD will communicate
vdd_dwParam	dd	0	; slowdown or acceleration parameter (fixed point 16.16, allows for 65536 through 1/65536 resolution)
vdd_wCounter	dw	0
vdd_wTemp	dw	0	; temporary

ISR_busy	db	0	; reentry guard for ISR

;

stack_top	db	100h dup(?)
stack_bottom	db	?

TEXT		ends


		end start
