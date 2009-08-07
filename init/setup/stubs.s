; Copyright 2009 Nick Johnson

section .data

helloworld:
	db "Hello, World!", 0x00

section .text

global _start
extern main
_start:
	mov eax, [helloworld]
	int 0x50
	jmp $

	call main
	jmp $

global fork
global exit
global sint
global sret
global mmap
global umap
global rmap
global fmap
global eout
global rirq

fork:
	int 0x40
	ret

exit:
	mov eax, [esp+4]
	int 0x41
	ret

sint:
	mov eax, [esp+4]
	mov esi, [esp+8]
	mov ebx, [esp+12]
	mov ecx, [esp+16]
	mov edx, [esp+20]
	mov edi, [esp+24]
	int 0x42
	ret

sret:
	mov eax, [esp+4]
	int 0x43
	ret

mmap:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	int 0x44
	ret

umap:
	mov eax, [esp+4]
	int 0x45
	ret

rmap:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	int 0x46
	ret

fmap:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	int 0x47
	ret

eout:
	mov eax, [esp+4]
	int 0x50
	ret

rirq:
	mov eax, [esp+4]
	int 0x51
	ret

global inb
global outb
global inw

inb:
	mov dx, [esp+4]
	mov eax, 0
	in al, dx
	ret

outb:
	mov dx, [esp+4]
	mov al, [esp+6]
	out dx, al
	ret
