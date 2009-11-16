; Copyright 2009 Nick Johnson

section .text

global _start
extern main
extern _cini
_start:
	call _cini
	call main
	int 0x51
