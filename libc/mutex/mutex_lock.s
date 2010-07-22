; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global mutex_lock
mutex_lock:
	mov edx, [esp+4]
	mov eax, 0
	mov ecx, 1
	
	lock cmpxchg [edx], cl

	jz .win
	mov eax, 0
	ret
.win:
	mov eax, 1
	ret
