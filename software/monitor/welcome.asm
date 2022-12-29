
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

	; Set MMU Master Control Register
	ld	hl, #0xBBE0
	OUTW	#0xF0



	; Write a startup word to RAM if possible
	ld	hl, #0x2000
	ld	b, #0x00
write_ram_inner:
	ld	a, #0x55
	ld	(hl), a
	inc	hl
	djnz	write_ram_inner



	; Set I/O Page Register to 0xFE
	ld	l, #0xFE
	ld	c, #0x08
	; LDCTL (C), HL
	.db	0xED
	.db	0x6E

	; Set Timer1 Configuration Register
	ld	a, #0xC0	; Continuous, Retrigger, IPA=0
	out	(0xE8), a

	; Set Timer1 Command/Status Register
	ld	a, #0xF8	; Enable, Software Gate, Software Trigger
	out	(0xE9), a

	; Set Timer1 Time Constant Register
	ld	hl, #0x27
	OUTW	#0xEA

	; Set UART Control Register
	ld	a, #0xC8
	out	(0x10), a

	; Set Transmit Control Register
	ld	a, #0x80
	out	(0x12), a


ld	l, #0x02
LDCTL	#0x10
.db 0xED
.db 0x9F
.db 0
.db 0
.db 0
.db 0


_main:

	ld	de, #msg
print_msg:
	ld	a, (de)
	ld	b, #0
	cp	b
	jp	Z, end_print
	out	(0x18), a
	inc	de

    wait:
	in	a, (0x12)
	bit	0, a
	jp	Z, wait

	jp	print_msg
	
end_print:

	;ret
	halt
; end _main

msg:	.ascii	"Welcome To Bread80!\n\n\0"

page_table:
	; User Descriptors
	.dw	0x000C
	.dw	0x001C
	.dw	0x1008
	.dw	0x1018
	.dw	0x1028
	.dw	0x1038
	.dw	0x1048
	.dw	0x1058
	.dw	0x1068
	.dw	0x1078
	.dw	0x1088
	.dw	0x1098
	.dw	0x10A8
	.dw	0x10B8
	.dw	0x10C8
	.dw	0x10D8

	; System Descriptors
	.dw	0x000C
	.dw	0x001C
	.dw	0x1008
	.dw	0x1018
	.dw	0x1028
	.dw	0x1038
	.dw	0x1048
	.dw	0x1058
	.dw	0x1068
	.dw	0x1078
	.dw	0x1088
	.dw	0x1098
	.dw	0x10A8
	.dw	0x10B8
	.dw	0x10C8
	.dw	0x10D8

; force the size to 512 bytes
	.org	0x200
	nop
