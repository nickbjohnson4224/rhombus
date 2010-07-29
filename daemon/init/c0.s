; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern dict_init
extern _init

_start:

	call dict_init

	push dword 1
	call _init
