
#ifndef _Z280_H
#define _Z280_H

#define SET_PAGE_REG(page)		\
	__asm__(			\
	"ld	l, #" #page "\n"	\
	"ld	c, #0x08\n"		\
	".db	#0xED\n"		\
	".db	#0x6E\n"		\
	)

#define LDCTL(address, value)		\
	__asm__(			\
	"ld	l, #" #value "\n"	\
	"ld	c, #" #address "\n"	\
	".db	#0xED\n"		\
	".db	#0x6E\n"		\
	)

#define OUT_u8(address, value)		\
	__asm__(			\
	"ld	a, #" #value "\n"	\
	"out	(" #address "), a\n"	\
	)

#define OUT_u16(address, value)		\
	__asm__(			\
	"ld	hl, #" #value "\n"	\
	"ld	c, #" #address "\n"	\
	".db	#0xED\n"		\
	".db	#0xBF\n"		\
	)

#endif

