; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global _recv

_recv:
	push edi

	mov ecx, [esp+8]
	mov edx, [esp+12]
	mov edi, [esp+16]
	int 0x43

	pop edi
	ret
