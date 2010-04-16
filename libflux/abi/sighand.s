; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

global signal_handle

extern signal_redirect
extern _drop

signal_handle:
	push ebx
	push edi
	push esi
	call signal_redirect
	add esp, 12

	call _drop
