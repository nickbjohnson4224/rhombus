; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global mutex_spin
mutex_spin:
	mov edx, [esp+4]
	mov eax, 0
	mov ecx, 1
	
.retry:
	lock cmpxchg [edx], cl
	jnz .retry

	ret
