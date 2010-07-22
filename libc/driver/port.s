; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

section .text

global inb
global inw
global ind

inb:
	mov dx, [esp+4]
	xor eax, eax
	in al, dx
	ret

inw:
	mov dx, [esp+4]
	xor eax, eax
	in ax, dx
	ret

ind:
	mov dx, [esp+4]
	in eax, dx
	ret

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

global iodelay

iodelay:
	mov ecx, [esp+4]

.loop:
	mov dx, 0x80
	mov al, 0x00
	out dx, al
	cmp ecx, 0
	dec ecx
	jne .loop

	ret
