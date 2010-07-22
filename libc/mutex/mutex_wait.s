; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global mutex_wait
mutex_wait:
	mov edx, [esp+4]

.retry:
	lock mov al, [edx]
	cmp al, 0
	jne .retry

	ret
