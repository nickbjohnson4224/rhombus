; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

; ***************************************************************************
; space_load
;
; Loads a new address space. Nuff said.

space_curr:
	dd 0

global space_load
space_load:
	mov eax, [esp+4]
	mov edx, [space_curr]
	cmp eax, edx
	je .cached
	mov cr3, eax
	mov [space_curr], eax
	.cached:
	ret
