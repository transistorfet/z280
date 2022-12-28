
	org	0000h
	jp	start

	org	0080h
	seek	0080h

start:
	ld	a, 0AAh
	ld	hl, 0FFFFh
	ld	(hl), a
	halt

	; Set I/O Page Register to 0xFE
	ld	l, 0FEh
	ld	c, 08h
	; LDCTL (C), HL
	db	0EDh
	db	06Eh

	; Set UART Control Register
	ld	a, 0C8h
	out	(010h), a

	; Set Transmit Control Register
	ld	a, 080h
	out	(012h), a

send:
	; Send byte
	ld	a, 0xAAh
	out	(018h), a
	jp	send

	;ld	sp, 0200h
	;call	_main
	halt

_main:
	ld	a, 01h
	out	(00FFh), a

	ld	de, msg

loop:
	ld	a, (de)
	cp	0
	jp	Z, loop_end
	out	(00F1h), a
	inc	de
	jp	loop
	
loop_end:

	ret
; end _main

msg:	defm	"Welcome To Bread80!\n\n\0"


; force the size to 512 bytes
	seek	0200h
	nop
