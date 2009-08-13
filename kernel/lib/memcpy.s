[BITS 32]

global memcpy
memcpy:
	mov edi, [esp+4]
	mov esi, [esp+8]
	mov ecx, [esp+12]
	
	rep movsb

	mov eax, edi
	ret

global memset
memset:
	mov edi, [esp+4]
	mov eax, [esp+8]
	mov ecx, [esp+12]

	rep stosb

	mov eax, edi
	ret
