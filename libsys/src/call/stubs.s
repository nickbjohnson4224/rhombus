; Copyright 2009 Nick Johnson

section .text

global fork_call
global exit_call
global sint_call
global sret_call
global mmap_call
global umap_call
global rmap_call
global fmap_call
global eout
global rirq

fork_call:
	int 0x40
	ret

exit_call:
	mov eax, [esp+4]
	int 0x41
	ret

sint_call:
	mov eax, [esp+4]
	mov esi, [esp+8]
	mov ebx, [esp+12]
	mov ecx, [esp+16]
	mov edx, [esp+20]
	mov edi, [esp+24]
	int 0x42
	ret

sret_call:
	mov eax, [esp+4]
	int 0x43
	ret

mmap_call:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	int 0x44
	ret

umap_call:
	mov eax, [esp+4]
	int 0x45
	ret

rmap_call:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	int 0x46
	ret

fmap_call:
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
