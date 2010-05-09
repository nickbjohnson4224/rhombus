; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _on_event

extern on_event
extern _drop

_on_event:
	
	push ebx
	push edi
	push esi
	call on_event
	add esp, 12

	call _drop
