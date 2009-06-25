section .data

helloworld:
	db "Hello, world"
	db 0x0D, 0x00

section .text

extern setup

global main
main:
;	cmp esi, 0
;	jne .end
;	call setup
	
	mov eax, helloworld
	int 0x50
	cmp esi, 0
	je setup
	jmp $
