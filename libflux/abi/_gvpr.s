; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _gvpr

_gvpr:
	
	mov eax, [esp+4]
	int 0x44

	ret
