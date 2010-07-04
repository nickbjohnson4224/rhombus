; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _svpr

_svpr:

	mov eax, [esp+4]
	int 0x45

	ret
