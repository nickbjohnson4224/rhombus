; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global _recv

_recv:
	
	mov ecx, [esp+4]
	mov edx, [esp+8]
	int 0x43

	ret
