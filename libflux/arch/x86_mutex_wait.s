; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_mutex_wait
x86_mutex_wait:
	mov edx, [esp+4]

.retry:
	lock mov eax, [edx]
	cmp eax, 0
	jne .retry

	ret
