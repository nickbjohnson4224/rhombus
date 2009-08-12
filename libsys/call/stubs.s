; Copyright 2009 Nick Johnson

section .text

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
global rsig

fork:
	mov ebx, 0
	int 0x40
	ret

exit:
	mov eax, [esp+4]
	int 0x41
	ret

sint:
	mov edi, [esp+4]
	mov esi, [esp+8]
	mov ebx, [esp+16]
	mov ecx, [esp+20]
	mov edx, [esp+24]
	mov eax, [esp+8]
	shl eax, 8
	or esi, eax
	mov eax, [esp+12]
	int 0x42
	ret

sret:
	mov ebx, [esp+4]
	int 0x43
	ret

mmap:
	mov edi, [esp+4]
	mov ecx, [esp+8]
	mov ebx, [esp+12]
	int 0x44
	ret

umap:
	mov esi, [esp+4]
	mov ecx, [esp+8]
	mov ebx, 0
	int 0x45
	ret

rmap:
	mov edi, [esp+4]
	mov esi, [esp+8]
	mov ecx, [esp+12]
	mov ebx, [esp+16]
	int 0x46
	ret

fmap:
	mov eax, [esp+4]
	mov edi, [esp+8]
	mov esi, [esp+12]
	mov ecx, [esp+16]
	mov ebx, [esp+20]
	int 0x47
	ret

eout:
	mov eax, [esp+4]
	int 0x52
	ret

rirq:
	mov eax, [esp+4]
	int 0x50
	ret

rsig:
	mov edi, [esp+4]
	mov eax, [esp+8]
	int 0x48
	ret
