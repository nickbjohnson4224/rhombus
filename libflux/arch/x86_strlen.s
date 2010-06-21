; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_strlen
x86_strlen:
	push edi
	xor ecx, ecx
	mov edi, [esp+8]
	not ecx
	xor al, al
	cld
	repne scasb
	not ecx
	pop edi
	lea eax, [ecx-1]
	ret
