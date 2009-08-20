[bits 32]

section .text

global push_call
global pull_call
global rirq_call
global lirq_call

push_call:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi

	mov eax, [ebp+4]
	mov edi, [ebp+8]
	mov esi, [ebp+12]
	mov ecx, [ebp+16]
	int 0x52

	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

pull_call:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi

	mov eax, [ebp+4]
	mov esi, [ebp+8]
	mov edi, [ebp+12]
	mov ecx, [ebp+16]
	int 0x53

	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

rirq_call:
	mov eax, [esp+4]
	int 0x50
	ret

lirq_call:
	mov eax, [esp+4]
	int 0x51
	ret
