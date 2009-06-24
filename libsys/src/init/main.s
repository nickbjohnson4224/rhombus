section .text

extern setup

global main
main:
	cmp esi, 0
	jne .end
	call setup

.end:
	jmp .end
