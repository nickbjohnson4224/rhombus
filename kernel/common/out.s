; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global outb
global outw
global outd

outb:
	mov dx, [esp+4]
	mov al, [esp+8]
	out dx, al
	ret

outw:
	mov dx, [esp+4]
	mov ax, [esp+8]
	out dx, ax
	ret

outd:
	mov dx, [esp+4]
	mov eax, [esp+8]
	out dx, eax
	ret
