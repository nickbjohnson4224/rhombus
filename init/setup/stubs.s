; Copyright 2009 Nick Johnson

section .data

section .text

global _start
extern init
_start:
	call init
	jmp $

global fork
global exit
global sint
global sret
global mmap
global umap
global push
global pull
global eout
global rirq
global rsig
global lsig

fork:
	push ebx

	mov ebx, 0
	int 0x40

	pop ebx
	ret

exit:
	mov eax, [esp+4]
	int 0x41
	ret

sint:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi
	push ebx

	mov edi, [ebp+4]
	mov esi, [ebp+8]
	mov ebx, [ebp+16]
	mov ecx, [ebp+20]
	mov edx, [ebp+24]
	mov eax, [ebp+28]
	shl eax, 8
	or esi, eax
	mov eax, [ebp+12]
	int 0x42

	pop ebx
	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

sret:
	mov ebx, [esp+4]
	int 0x43
	ret

mmap:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push ebx

	mov edi, [ebp+4]
	mov ecx, [ebp+8]
	mov ebx, [ebp+12]
	int 0x44

	pop ebx
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

umap:
	push ebp
	mov ebp, esp
	add ebp, 4

	push esi
	push ebx

	mov esi, [ebp+4]
	mov ecx, [ebp+8]
	mov ebx, 0
	int 0x45

	pop ebx
	pop esi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

push:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi

	mov eax, [ebp+4]
	mov edi, [ebp+8]
	mov esi, [ebp+12]
	mov ecx, [ebp+16]
	int 0x52

	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

pull:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi

	mov eax, [ebp+4]
	mov esi, [ebp+8]
	mov edi, [ebp+12]
	mov ecx, [ebp+16]
	int 0x53

	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

rirq:
	mov eax, [esp+4]
	int 0x50
	ret

rsig:
	push edi

	mov edi, [esp+8]
	mov eax, [esp+12]
	int 0x46

	pop edi
	ret

lsig:
	push edi

	mov edi, [esp+8]
	int 0x47

	pop edi
	ret

eout:
	mov eax, [esp+4]
	int 0x54
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
	mov al, [esp+8]
	out dx, al
	ret

global print_stub
extern print_handler
print_stub:
	push ecx
	push edx
	push esi
	
	call print_handler

	add esp, 12

	push dword 3
	call sret
