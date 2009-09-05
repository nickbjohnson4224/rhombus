[bits 32]

global set_stack
set_stack:
	mov eax, [esp]
	mov edx, [esp+4]
	mov esp, edx
	push eax
	ret	

global exec_jump
exec_jump:
	mov eax, [esp+4]
	mov ebp, [esp+8]
	mov esp, ebp
	push eax
	ret
