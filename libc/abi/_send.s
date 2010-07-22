; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global _send

_send:

	mov ecx, [esp+4]
	mov eax, [esp+8]

	int 0x40

	ret
