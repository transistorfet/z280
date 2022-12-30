
#include "z280.h"


/*
// Initialize the UART and Timer1
void init_uart()
{
	SET_PAGE_REG(0xFE);

	// Set Timer1 Configuration Register
	OUT_u8(0xE8, 0xC0);	// Continuous, Retrigger, IPA=0

	// Set Timer1 Command/Status Register
	OUT_u8(0xE9, 0xF8);	// Enable, Software Gate, Software Trigger

	// Set Timer1 Time Constant Register
	OUT_u16(0xEA, 0x27);

	// Set UART Control Register
	OUT_u8(0x10, 0xC8);

	// Set Transmit Control Register
	OUT_u8(0x12, 0x80);
}
*/

// Initialize the UART and Timer1
void init_uart() __naked
{
	__asm
	.include "../../include/z280.inc"

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
	__endasm;
}


// Print a null-terminated string
void print_msg(const char *msg) __naked
{
	__asm
	ld	iy, #2
	add	iy, sp
	ld	d, 1(iy)
	ld	e, 0(iy)

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
	__endasm;
}

void main()
{
	init_uart();

	print_msg("Welcome To C on a Z280!");

	return;
}
 
