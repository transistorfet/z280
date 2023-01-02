
	.include "../../include/z280.inc"

	.area	CODE	(ABS)

	.globl	_main
	.org	0x0000
	jp	start

	.org	0x0080

start:
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

	; Initialize the stack pointer to the top of address space, where ram is mapped
	ld	sp, #0x0000

	; Clear the stack to make debugging easier
	ld	bc, #0
	ld	hl, #0xF000
	ld	a, #0
clear:
	ld	(hl), a
	inc	hl
	sbc	hl, bc
	jp	NZ, clear

	call	_main
	halt


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

