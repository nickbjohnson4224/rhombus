section .text

extern sret

global write_handler_stub
extern write_handler
write_handler_stub:
	push ecx
	push edx
	push esi
	call write_handler
	add esp, 12
	
	mov eax, 3
	push eax
	call sret

global read_handler_stub
extern read_handler
read_handler_stub:
	push edx
	push esi
	call read_handler
	add esp, 8
	
	mov eax, 3
	push eax
	call sret
