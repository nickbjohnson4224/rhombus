; Copyright 2009, 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

#define ASM
#include <flux/config.h>

section .text

global page_flush_full
page_flush_full:
	mov eax, cr3
	mov cr3, eax
	ret

global page_flush
page_flush:
	mov eax, [esp+4]
	invlpg [eax]
	ret

global map_load
map_load:
	mov eax, [esp+4]
	mov edx, cr3
	cmp edx, eax
	je .same
	mov cr3, eax
.same:
	ret

global get_cr3
get_cr3:
	mov eax, cr3
	ret

global get_cr2
get_cr2:
	mov eax, cr2
	ret

global idle
idle:
	mov eax, [esp]
	sti
	hlt

global halt:
halt:
	cli
	hlt

global read_tsc
read_tsc:
	rdtsc
	ret

global get_eflags
get_eflags:
	pushf
	pop eax
	ret
