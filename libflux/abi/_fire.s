; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _fire

_fire:
	push ebx

	mov eax, [esp+8]
	mov ecx, [esp+12]
	mov ebx, [esp+16]
	mov edx, [esp+20]
	int 0x40

	pop ebx
	ret
