; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global _send

_send:
	push ebx

	mov ecx, [esp+8]
	mov eax, [esp+12]
	mov ebx, [esp+16]

	int 0x40

	pop ebx
	ret
