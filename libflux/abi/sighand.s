; Copyright 2010 Nick Johnson

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
