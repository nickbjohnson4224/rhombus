; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

extern fpu_p

global fpu_save
fpu_save:
	mov eax, [fpu_p]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	fxsave [ecx]
.blank:
	ret
