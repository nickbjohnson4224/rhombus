; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_strncpy
x86_strncpy:
	push esi
	push edi

	xor eax, eax
	mov edi, [esp+12]
	mov esi, [esp+16]
	mov ecx, [esp+20]

	cld

.loop:
	lodsb
	stosb
	test al, al
	jz .end
	dec ecx
	jnz .loop
.end:

	mov eax, [esp+12]
	pop edi
	pop esi
	ret
