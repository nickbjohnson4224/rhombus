; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _exit
extern send

_exit:
	push dword 0
	mov eax, 1
	int 0x4C
	push eax
	push dword 7
	call send
	add esp, 12

	mov eax, [esp+4]
	int 0x49
