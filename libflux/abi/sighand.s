; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global sighand

extern sigredirect
extern _drop

sighand:
	push ebx
	push edi
	push esi
	call sigredirect
	add esp, 12

	call _drop
