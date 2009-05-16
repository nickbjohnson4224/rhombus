section .text

global main
main:
	mov eax, 0x42242442
	xor ebx, ebx
	xor ecx, ecx

.loop:
	call increment
	cmp ebx, 10000
	jg .end
	jmp .loop

.end:
	int 0x40
	xor ebx, ebx
	jmp .loop

increment:
	inc ebx
	ret
