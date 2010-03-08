; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

; ***************************************************************************
; space_save
; 
; Returns currently loaded address space.

extern space_curr ; in space_load.s

global space_save
space_save:
	mov eax, [space_curr]
	cmp eax, 0
	jne .cached
	mov eax, cr3
	.cached:
	ret
