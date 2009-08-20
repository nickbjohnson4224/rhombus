[bits 32]

section .text

global push
global pull

push:
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

pull:
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
