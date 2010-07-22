; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global mutex_test
mutex_test:
	mov edx, [esp+4]
	
	lock mov al, [edx]

	ret
