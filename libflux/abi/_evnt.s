; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _evnt

_evnt:

	mov ecx, [esp+4]
	mov edx, [esp+8]
	int 0x42

	ret
