; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

extern kstack
extern int_handler
extern thread_load

global thread_save
thread_save:
	pusha

	xor eax, eax
	mov ax, ds
	push eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	sub esp, 512

	mov ebp, esp
	mov esp, (kstack + 0x1FF0)

	push ebp
	call int_handler
	mov esp, eax

	jmp thread_load
