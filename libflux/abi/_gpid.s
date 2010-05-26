; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _gpid

_gpid:
	int 0x4C
	ret
