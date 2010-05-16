; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern main
extern fdinit
extern _cini
extern _exit

_start:
	push edx ; envp
	push eax ; argv
	push ecx ; argc

	call fdinit
	call _cini

	call main

	push eax ; return value of main
	call _exit
