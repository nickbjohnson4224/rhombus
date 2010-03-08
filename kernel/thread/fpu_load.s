; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

exterm fpu_p

global fpu_load
fpu_load:
	mov eax, [fpu_p]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	fxrstor [ecx]
.blank:
	ret
