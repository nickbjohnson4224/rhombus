; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _exit

_exit:
	mov eax, [esp+4]
	int 0x49
