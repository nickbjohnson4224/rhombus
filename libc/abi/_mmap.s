; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _mmap

_mmap:
	push ebx

	mov ebx, [esp+8]
	mov ecx, [esp+12]
	mov edx, [esp+16]
	int 0x50

	pop ebx
	ret
