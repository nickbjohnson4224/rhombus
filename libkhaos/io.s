[bits 32]

section .text

global inb
global inw
global ind

inb:
	mov dx, [esp+4]
	mov al, [esp+8]
	in al, dx
	ret

inw:
	mov dx, [esp+4]
	mov ax, [esp+8]
	in ax, dx
	ret

ind:
	mov dx, [esp+4]
	mov eax, [esp+8]
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
