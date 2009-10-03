; Copyright 2009 Nick Johnson

section .text

global _start
extern init
_start:
	call init
	jmp $

global sig_handler
extern sret_call
extern csig_handler
sig_handler:
	lea ecx, [csig_handler]
	push eax
	push edi
	call ecx
	mov eax, 3
	push eax
	call sret_call
