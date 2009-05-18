section .text

extern setup

global main
main:
	call setup_sig
	xor ebx, ebx
	xor edx, edx
	int 0x40
	nop
	call setup

.loop:
	call increment
	int 0x40
	jmp .loop

increment:
	call increment2
	ret

increment2:
	inc ebx
	ret

signal_handler:
	jmp signal_handler
	int 0x43

setup_sig:
	mov eax, 0xF3FFF000
	lea ebx, [signal_handler]
	mov [eax], ebx
	ret

global fork
fork:
	int 0x40
	ret

global sint
sint:
	pop eax
	pop esi
	pop ebx
	pop ecx
	pop edx
	pop edi
	int 0x42
	ret

global sret
sret:
	int 0x43
	ret
