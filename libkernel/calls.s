global fork_call
global exit_call
global sint_call
global sret_call
global mmap_call
global umap_call
global rsig_call
global lsig_call
global mmgc_call

fork_call:
	push ebx

	mov ebx, 0
	int 0x40

	pop ebx
	ret

exit_call:
	mov eax, [esp+4]
	int 0x41
	ret

sint_call:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push esi
	push ebx

	mov edi, [ebp+4]
	mov esi, [ebp+8]
	mov ebx, [ebp+16]
	mov ecx, [ebp+20]
	mov edx, [ebp+24]
	mov eax, [ebp+28]
	shl eax, 8
	or esi, eax
	mov eax, [ebp+12]
	int 0x42

	pop ebx
	pop esi
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

sret_call:
	mov ebx, [esp+4]
	int 0x43
	ret

mmap_call:
	push ebp
	mov ebp, esp
	add ebp, 4

	push edi
	push ebx

	mov edi, [ebp+4]
	mov ecx, [ebp+8]
	mov ebx, [ebp+12]
	int 0x44

	pop ebx
	pop edi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

umap_call:
	push ebp
	mov ebp, esp
	add ebp, 4

	push esi
	push ebx

	mov esi, [ebp+4]
	mov ecx, [ebp+8]
	mov ebx, 0
	int 0x45

	pop ebx
	pop esi

	sub ebp, 4
	mov esp, ebp
	pop ebp
	ret

rsig_call:
	push edi
	push esi

	mov edi, [esp+12]
	mov eax, [esp+16]
	int 0x46

	pop esi
	pop edi
	ret

lsig_call:
	push edi

	mov edi, [esp+8]
	int 0x47

	pop edi
	ret

mmgc_call:
	int 0x48
	ret
