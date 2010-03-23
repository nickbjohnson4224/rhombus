; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global space_load

space_load:
	mov eax, [esp+4]
	mov edx, cr3
	cmp edx, eax
	je .same
	mov cr3, eax
	.same:
	ret
