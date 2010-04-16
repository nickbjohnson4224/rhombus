; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_mutex_free
x86_mutex_free:
	mov edx, [esp+4]

	xor eax, eax
	lock mov [edx], eax

	ret
