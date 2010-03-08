; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global thread_load

thread_load:
	add esp, 512

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	popa
	add esp, 8
	iret
