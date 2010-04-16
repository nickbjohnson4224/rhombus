; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global x86_mutex_spin
x86_mutex_spin:
	mov edx, [esp+4]
	mov eax, 0
	mov ecx, 1
	
.retry:
	lock cmpxchg [edx], ecx
	jnz .retry

	ret
