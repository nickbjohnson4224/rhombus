; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global x86_memcmp
x86_memcmp:
	push esi
	push edi

	xor eax, eax
	mov esi, [esp+12]
	mov edi, [esp+16]
	mov ecx, [esp+20]

.loop:
	mov al, [esi]
	mov dl, [edi]
	cmp al, dl
	jne .end
	inc esi
	inc edi
	dec ecx
	jnz .loop
.end:
	sub al, dl
	
	pop edi
	pop esi
	ret
