; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_memclr
x86_memclr:
	push edi

	mov edi, [esp+8]
	mov ecx, [esp+12]
	xor eax, eax

	cld

	rep stosb

	mov eax, edi
	pop edi
	ret
