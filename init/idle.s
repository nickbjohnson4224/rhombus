; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global idle

idle:
	hlt
	jmp idle
