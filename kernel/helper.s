; Copyright 2009, 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

[bits 32]

#define ASM
#include <arch.h>

section .text

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
	jmp $

global halt:
halt:
	cli
	hlt

global read_tsc
read_tsc:
	rdtsc
	ret

global get_cpuid_flags
get_cpuid_flags:
	push ebx
	mov eax, 1
	cpuid
	mov eax, edx
	pop ebx
	ret

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
	dw 0x2FF
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

