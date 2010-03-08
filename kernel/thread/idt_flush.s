; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

idt_ptr:
	dw 0x3FF
	dd 0

global idt_flush
idt_flush:
	push ebx

	mov eax, [esp+4]
	mov ebx, idt_ptr
	mov [ebx+2], eax
	
	lidt [eax]

	pop ebx
	ret
