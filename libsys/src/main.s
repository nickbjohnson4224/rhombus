section .text

extern setup

global main
main:
	int 0x40
	call setup

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
	mov eax, [esp+4]
	int 0x43
	ret

global eout
eout:
	mov eax, [esp+4]
	int 0x44
	ret
