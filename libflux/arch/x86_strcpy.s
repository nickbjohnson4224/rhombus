; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_strcpy
x86_strcpy:
	push esi
	push edi

	mov edi, [esp+12]
	mov esi, [esp+16]

	cld

.loop:
	lodsb
	stosb
	test al, al
	jnz .loop

	mov eax, [esp+12]

	pop edi
	pop esi
	ret
