; Copyright 2009 Nick Johnson

[bits 32]

#include <config.h>

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

section .itext

global get_eflags
get_eflags:
	pushf
	pop eax
	ret

global tss_flush
tss_flush:
	mov ax, 0x28
	ltr ax
	ret

section .data
extern idt
idt_ptr:
	dw 0x3FF
	dd idt

section .text
align 4

global idt_flush
idt_flush:
	mov eax, idt_ptr
	lidt [eax]
	ret
