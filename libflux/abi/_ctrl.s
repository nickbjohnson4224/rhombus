; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _pctl

_pctl:
	mov eax, [esp+4]
	mov edx, [esp+8]
	mov ecx, [esp+12]
	int 0x4A

	ret
