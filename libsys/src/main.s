section .text

global main
main:
	call setup_sig
	xor ebx, ebx
	xor edx, edx
	int 0x40

.loop:
	call increment
	cmp ebx, 0x1000
	jle .loop

	mov eax, 0
	mov ebx, 0x42
	mov ecx, 0x4224
	mov edx, 0x42242442
	mov esi, 0
	mov edi, 0
	int 0x42

	xor ebx, ebx

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
