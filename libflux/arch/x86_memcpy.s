; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_memcpy
x86_memcpy:
	push esi
	push edi

	mov edi, [esp+12]
	mov esi, [esp+16]
	mov ecx, [esp+20]

	rep movsb

	mov eax, edi
	pop edi
	pop esi
	ret
