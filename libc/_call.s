global sighand
extern sigredirect
sighand:
	push ebx
	push edi
	push esi
	call sigredirect
	add esp, 12

	call _drop

global _fire
global _drop
global _hand
global _ctrl
global _info
global _mmap
global _fork
global _exit

_fire:
	push ebx

	mov eax, [esp+8]
	mov ecx, [esp+12]
	mov ebx, [esp+16]
	int 0x60

	pop ebx
	ret

_drop:
	int 0x61
	
_hand:
	mov eax, [esp+4]
	int 0x62

	ret

_ctrl:
	mov eax, [esp+4]
	mov edx, [esp+8]
	int 0x63

	ret

_info:
	mov eax, [esp+4]
	int 0x64

	ret

_mmap:
	push ebx

	mov ebx, [esp+8]
	mov ecx, [esp+12]
	mov edx, [esp+16]
	int 0x65

	pop ebx
	ret

_fork:
	int 0x66

	ret

_exit:
	mov eax, [esp+4]
	int 0x67
