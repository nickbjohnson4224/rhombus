; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_strlcpy
x86_strlcpy:
	push esi
	push edi

	mov edi, [esp+12]
	mov esi, [esp+16]
	mov ecx, [esp+20]
	jz .zsize
	dec ecx

.loop:
	lodsb
	stosb
	test al, al
	jz .end
	dec ecx
	jnz .loop
	xor al, al
	stosb
.end:

.zsize:
	mov eax, [esp+20]
	sub eax, ecx
	
	pop edi
	pop esi
	ret
