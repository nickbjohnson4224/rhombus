section .text

global main
main:
	mov eax, 0x42242442
	xor ebx, ebx
.loop:
	inc ebx
	jmp .loop
