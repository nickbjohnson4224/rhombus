section .text

extern setup

global main
main:
	cmp esi, 0
	jne .end
	int 0x40
	call setup

.end:
	jmp .end

global fork
fork:
	int 0x40
	ret

global sint
sint:
	mov eax, [esp+4]
	mov esi, [esp+8]
	mov ebx, [esp+12]
	mov ecx, [esp+16]
	mov edx, [esp+20]
	mov edi, [esp+24]
	int 0x42
	ret

global sret
sret:
	int 0x43
	ret

global mmap
mmap:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	int 0x44
	ret

global umap
umap:
	mov eax, [esp+4]
	int 0x45
	ret

global rmap
rmap:
	mov eax, [esp+4]
	mov ebx, [esp+8]
	mov ecx, [esp+12]
	int 0x46
	ret

global eout
eout:
	mov eax, [esp+4]
	int 0x50
	ret

global exit
exit:
	mov eax, [esp+4]
	int 0x41
	ret
