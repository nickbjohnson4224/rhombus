; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _mail

_mail:

	mov ecx, [esp+4]
	mov edx, [esp+8];
	mov eax, [esp+12];
	int 0x43

	ret
