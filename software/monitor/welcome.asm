
	.include "z280.inc"

	.area	CODE	(ABS)
	.org	0x0000
	jp	start

	.org	0x0080

start:
	ld	sp, #0x2000

	; Set I/O Page Register to 0xFF
	ld	l, #0xFF
	LDCTL	#0x08

	; Set MMU Page Descriptor Pointer
	ld	a, #0
	out	(#0xF1), a

	; Set sequential page registers
	ld	b, #0x20	; 32 pages
	ld	hl, #page_table
	OTIRW	#0xF4

	ld	l, #0x02
	LDCTL	#0x10

.db 0xED
.db 0x9F
.db 0
.db 0
.db 0
.db 0

	; Set MMU Master Control Register
	ld	h, #0xBB		; Enabled the MMU
	ld	l, #0xE0
	OUTW	#0xF0


	ld	b, #0x30
delay:
	djnz	delay

	halt


	; Write a startup word to RAM if possible
	ld	hl, #0x2000
	ld	a, #0x55
	ld	(hl), a
	inc	hl
	inc	hl
	ld	a, #0xAA
	ld	(hl), a




	; Set I/O Page Register to 0xFE
	ld	l, #0xFE
	ld	c, #0x08
	; LDCTL (C), HL
	.db	0xED
	.db	0x6E

	; Set UART Control Register
	ld	a, #0xC8
	out	(0x10), a

	; Set Transmit Control Register
	ld	a, #0x80
	out	(0x12), a



send:
	; Send byte
	ld	a, #0xAA
	out	(0x18), a
	;jp	send

	;call	_main
	halt

_main:
	ld	a, #0x01
	out	(0x00FF), a

	ld	de, #msg

loop:
	ld	a, (de)
	ld	b, #0
	cp	b
	jp	Z, loop_end
	out	(0x00F1), a
	inc	de
	jp	loop
	
loop_end:

	ret
; end _main

msg:	.ascii	"Welcome To Bread80!\n\n\0"

page_table:
	; User Descriptors
	.dw	0x0800
	.dw	0x1800
	.dw	0x0C10
	.dw	0x1C10
	.dw	0x2C10
	.dw	0x3C10
	.dw	0x4C10
	.dw	0x5C10
	.dw	0x6C10
	.dw	0x7C10
	.dw	0x8C10
	.dw	0x9C10
	.dw	0xAC10
	.dw	0xBC10
	.dw	0xCC10

	; System Descriptors
	.dw	0x0800
	.dw	0x1800
	.dw	0x0C10
	.dw	0x1C10
	.dw	0x2C10
	.dw	0x3C10
	.dw	0x4C10
	.dw	0x5C10
	.dw	0x6C10
	.dw	0x7C10
	.dw	0x8C10
	.dw	0x9C10
	.dw	0xAC10
	.dw	0xBC10
	.dw	0xCC10

; force the size to 512 bytes
	.org	0x200
	nop
