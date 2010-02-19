; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern main
extern _cini
_start:
	call _cini
	call main
	int 0x51
