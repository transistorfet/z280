
IO_PAGE_REG 	= #0x08
 
.macro 	OUTW	address
	ld	c, address
	.db	0xED
	.db 	0xBF
.endm

.macro	OTIRW	address
	ld	c, address
	.db	0xED
	.db	0x93
.endm

.macro	LDCTL	address
	ld	c, address
	.db	#0xED
	.db	#0x6E
.endm

