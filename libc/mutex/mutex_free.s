; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global mutex_free
mutex_free:
	mov edx, [esp+4]

	xor eax, eax
	mov [edx], al

	ret
