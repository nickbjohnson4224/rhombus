; Copyright 2009 Nick Johnson

section .text

global _start
extern init
_start:
	call init
	jmp $
