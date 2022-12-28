
	include "z280.inc"

	org	0000h
	jp	start

	org	0080h
	seek	0080h

start:
	ld	sp, 02000h

	; Set I/O Page Register to 0xFF
	ld	l, 0FFh
	ld	c, 08h
	; LDCTL (C), HL
	db	0EDh
	db	06Eh

	; Set MMU Page Descriptor Pointer
	ld	a, 00h
	out	(0F1h), a

	; Set sequential page registers
	ld	b, 20h	; 32 pages
	ld	hl, page_table
	OTIRW	0F4h


	; Set MMU Master Control Register
	ld	h, 0BBh		; Enabled the MMU
	ld	l, 0E0h
	OUTW	0F0h



	; Write a startup word to RAM if possible
	ld	hl, 2000h
	ld	a, 055h
	ld	(hl), a
	inc	hl
	inc	hl
	ld	a, 0AAh
	ld	(hl), a




	; Set I/O Page Register to 0xFE
	ld	l, 0FEh
	ld	c, 08h
	; LDCTL (C), HL
	db	0EDh
	db	06Eh

	; Set UART Control Register
	ld	a, 0C8h
	out	(010h), a

	; Set Transmit Control Register
	ld	a, 080h
	out	(012h), a



send:
	; Send byte
	ld	a, 0AAh
	out	(018h), a
	jp	send

	ld	b, 50h
delay:
	djnz	delay

	halt

	;call	_main
	halt

_main:
	ld	a, 01h
	out	(00FFh), a

	ld	de, msg

loop:
	ld	a, (de)
	cp	0
	jp	Z, loop_end
	out	(00F1h), a
	inc	de
	jp	loop
	
loop_end:

	ret
; end _main

msg:	defm	"Welcome To Bread80!\n\n\0"

page_table:
	; User Descriptors
	dw	00800h
	dw	01800h
	dw	00C10h
	dw	01C10h
	dw	02C10h
	dw	03C10h
	dw	04C10h
	dw	05C10h
	dw	06C10h
	dw	07C10h
	dw	08C10h
	dw	09C10h
	dw	0AC10h
	dw	0BC10h
	dw	0CC10h

	; System Descriptors
	dw	00800h
	dw	01800h
	dw	00C10h
	dw	01C10h
	dw	02C10h
	dw	03C10h
	dw	04C10h
	dw	05C10h
	dw	06C10h
	dw	07C10h
	dw	08C10h
	dw	09C10h
	dw	0AC10h
	dw	0BC10h
	dw	0CC10h

; force the size to 512 bytes
	seek	0200h
	nop
