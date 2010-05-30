; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global _start
extern main
extern _cini
extern _exit
extern _fini

_start:
	mov eax, 0xE0010004
	push eax ; argv
	mov eax, 0xE0010000
	mov edx, [eax]
	push edx ; argc

	call _fini ; libflux initialization
	call _cini ; libc initialization

	call main

	push eax ; return value of main
	call _exit
