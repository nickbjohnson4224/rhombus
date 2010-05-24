; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern main
extern _cini
extern _exit

_start:
	push eax ; argv
	push ecx ; argc

	call _cini ; libc initialization

	call main

	push eax ; return value of main
	call _exit
