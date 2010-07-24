; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global _start
extern main
extern _init
extern _exit

_start:
	mov eax, 0xD0010004
	push eax ; argv
	mov eax, 0xD0010000
	mov edx, [eax]
	push edx ; argc

	call _init ; libc initialization

	call main

	push eax ; return value of main
	call _exit
