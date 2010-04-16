; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_mutex_test
x86_mutex_test:
	mov edx, [esp+4]
	
	lock mov eax, [edx]

	ret
