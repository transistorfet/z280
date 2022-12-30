;
; A simple demostration program for booting the basic system
;

	.include "z280.inc"

	.area	CODE	(ABS)
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

	; Now that we can access ram, call the main function
	call	_main
	halt

_main:
	call	init_uart

	ld	de, #msg
	call	print_msg
	ret

; Initialize the UART and Timer1
init_uart:
	; Set I/O Page Register to 0xFE
	ld	l, #0xFE
	LDCTL	#0x08

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

	ret


; Print a null-terminated string in DE
print_msg:
	; Set I/O Page Register to 0xFE
	ld	l, #0xFE
	LDCTL	#0x08

    _print_next:
	ld	a, (de)
	ld	b, #0
	cp	b
	jp	Z, _print_end
	out	(0x18), a
	inc	de

    _print_wait:
	in	a, (0x12)
	bit	0, a
	jp	Z, _print_wait

	jp	_print_next
    _print_end:

	ret

; Write data into ram to make sure it's mapped correctly
ram_test:
	; Write a startup word to RAM if possible
	ld	hl, #0x2000
	ld	b, #0x00
write_ram_inner:
	ld	a, #0x55
	ld	(hl), a
	inc	hl
	djnz	write_ram_inner

	ret


;;;;;;;;;;;;;;;;;;;
;;;; Data      ;;;;
;;;;;;;;;;;;;;;;;;;

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
