; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern main
extern _fini
extern _cini
extern _exit

_start:
	push edx ; envp
	push eax ; argv
	push ecx ; argc

	call _fini ; libflux initialization
	call _cini ; libc initialization

	call main

	push eax ; return value of main
	call _exit
