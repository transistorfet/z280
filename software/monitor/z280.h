
#ifndef _Z280_H
#define _Z280_H

#define SET_PAGE_REG(page)	\
	__asm__(		\
	"ld	l, %0\n"	\
	"ld	c, #0x08\n"	\
	".db	#0xED\n"	\
	".db	#0x6E\n"	\
	: : "rm" (page)		\
	: "l", "c"		\
	)

#define OUT_u8(address, value)	\
	__asm__(		\
	"ld	a, %1\n"	\
	"out	(%0), a\n"	\
	: : "rm" (address), "rm" (value)	\
	)

#define OUT_u16(address, value)	\
	__asm__(		\
	"ld	hl, %1\n"	\
	"ld	c, %0\n"	\
	".db	#0xED\n"	\
	".db	#0xBF\n"	\
	: : "rm" (address), "rm" (value)	\
	)

#endif

