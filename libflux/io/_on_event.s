; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global _on_event

extern on_event
extern _drop

_on_event:
	
	push esi
	push edi
	call on_event
	add esp, 8

	call _drop
