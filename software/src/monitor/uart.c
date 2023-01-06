
#include <stdint.h>

#include "z280.h"
#include "uart.h"


struct _FILE {
    uint16_t number;
};

FILE __stdout = { 0 };


// Initialize the UART and Timer1
void init_uart()
{
	SET_PAGE_REG(0xFE);

	//// Set Timer1 Configuration Register
	//OUT_u8(0xE8, 0xC8);	// Continuous, Retrigger, IPA=8

	//// Set Timer1 Time Constant Register
	//OUT_u16(0xEA, 23);

	//// Set Timer1 Command/Status Register
	//OUT_u8(0xE9, 0xF8);	// Enable, Gate=1, Trigger=1

	// Set UART Control Register
	//OUT_u8(0x10, 0xE8);
	// TODO this is now using the external clock source at f/32
	OUT_u8(0x10, 0xE4);

	// Set Transmit Control Register
	OUT_u8(0x12, 0x80);

	// Set Receive Control Register
	OUT_u8(0x14, 0x80);
}

int putchar(int ch) __naked
{
	__asm
	push	hl
	__endasm;

	SET_PAGE_REG(0xFE);

	__asm
	pop	hl

	; Check the transmit buffer empty flag until 0
    0001$:
	in	a, (0x12)
	bit	0, a
	jp	Z, 0001$

	; Write the character to output
	ld	a, l
	out	(0x18), a

	ld	hl, #0

	ret
	__endasm;
}

int getchar() __naked
{
	SET_PAGE_REG(0xFE);

	__asm
    0001$:
	in	a, (0x14)
	bit	4, a
	jp	Z, 0001$

	; Check if an error occurred
	bit	0, a
	jp	NZ, 0002$

	; Read in the next character and return
	in	a, (0x16)
	ld	d, #0
	ld	e, a

	ret

    0002$:
	or	a, #0xF0
	ld	d, #0
	ld	e, a

	; Discard the error byte
	in	a, (0x16)

	; Reset the error bits
	ld	a, #0x80
	out	(0x14), a

	ld	de, #0

	ret
	__endasm;
}

int puts(const char *str)
{
    for (; *str != 0; str++) {
	putchar(*str);
    }
    putchar('\n');
    return 0;
}

int fputs(const char *str, FILE *stream)
{
    for (; *str != 0; str++) {
	putchar(*str);
    }
    return 0;
}

