section .text

extern setup

global main
main:
	call setup
	jmp $
