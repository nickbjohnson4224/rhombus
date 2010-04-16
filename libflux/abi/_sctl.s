; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _sctl

_sctl:
	mov eax, [esp+4]
	mov ecx, [esp+8]
	mov edx, [esp+12]
	int 0x42

	ret
