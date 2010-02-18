; Copyright 2009, 2010 Nick Johnson

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

global inb
global inw
global ind

inb:
	mov dx, [esp+4]
	xor eax, eax
	in al, dx
	ret

inw:
	mov dx, [esp+4]
	xor eax, eax
	in ax, dx
	ret

ind:
	mov dx, [esp+4]
	in eax, dx
	ret

global outb
global outw
global outd

outb:
	mov dx, [esp+4]
	mov al, [esp+8]
	out dx, al
	ret

outw:
	mov dx, [esp+4]
	mov ax, [esp+8]
	out dx, ax
	ret

outd:
	mov dx, [esp+4]
	mov eax, [esp+8]
	out dx, eax
	ret

