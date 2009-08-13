; Copyright 2009 Nick Johnson

[BITS 32]

global memcpy
memcpy:
	push esi
	push edi
	mov edi, [esp+12]
	mov esi, [esp+16]
	mov ecx, [esp+20]
	
	rep movsb

	mov eax, edi

	pop edi
	pop esi
	ret

global memset
memset:
	push edi
	mov edi, [esp+8]
	mov eax, [esp+12]
	mov ecx, [esp+16]

	rep stosb

	mov eax, edi
	
	pop edi
	ret
