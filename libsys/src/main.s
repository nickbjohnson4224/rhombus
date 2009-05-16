section .text

global main
main:
	xor ebx, ebx

.loop:
	call increment
	cmp ebx, 0x1000
	jle .loop
	xor ebx, ebx
	int 0x40
	jmp .loop

increment:
	call increment2
	ret

increment2:
	inc ebx
	ret
