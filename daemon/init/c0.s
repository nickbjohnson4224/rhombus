; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

section .text

global _start
extern main
extern dict_init
extern fdinit
extern _init
extern _exit

_start:
	push edx ; envp
	push eax ; argv
	push ecx ; argc

	call fdinit
	call dict_init
	call _init

	call main

	push eax ; return value of main
	call _exit
