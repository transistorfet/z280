 
OUTW:	macro	address
	ld	c, #address
	db	0EDh
	db 	0BFh
endm

OTIRW:	macro	address
	ld	c, #address
	db	0EDh
	db	093h
endm

