; Copyright 2009 Nick Johnson

; Multiboot stuff
MODULEALIGN equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ  -(MAGIC + FLAGS)

section .bss

STACKSIZE equ 0x1000
global init_stack
init_stack:
	resd STACKSIZE >> 2

section .tdata

; Initial kernel address space
global init_kmap
align 0x1000
init_kmap:
    dd 0x00000083	; Identity map first 4 MB
	times 991 dd 0	; Fill until 0xF8000000
	dd 0x00000083	; Map first 4 MB again in higher mem
	times 31 dd 0	; Fill remainder of map

section .data

global gdt
gdt:
align 0x1000
	dd 0x00000000, 0x00000000
	dd 0x0000FFFF, 0x00CF9A00
	dd 0x0000FFFF, 0x00CF9200
	dd 0x0000FFFF, 0x00CFFA00
	dd 0x0000FFFF, 0x00CFF200
	dd 0x00000000, 0x0000E900 ; This will become the TSS

gdt_ptr:
align 4
	dw 0x002F	; 47 bytes limit
	dd gdt 		; Points to *virtual* GDT

section .text

; Multiboot header
MultiBootHeader:
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

extern init
global start
start:
	mov ecx, init_kmap - 0xF8000000	; Get physical address of the kernel address space
	mov cr3, ecx	; Load address into CR3
	mov ecx, cr4
	mov edx, cr0
	or ecx, 0x00000010	; Set 4MB page flag
	or edx, 0x80000000	; Set paging flag
	mov cr4, ecx		; Return to CR4 (make 4MB pgs)
	mov cr0, edx		; Return to CR0 (start paging)
	jmp 0x08:.upper		; Jump to the higher half (in new code segment)

.upper:
	mov ecx, gdt_ptr	; Load (real) GDT pointer
	lgdt [ecx]			; Load new GDT

	mov ecx, 0x10		; Reload all kernel data segments
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx

	mov esp, (init_stack + STACKSIZE)	; Setup init stack
	mov ebp, (init_stack + STACKSIZE)	; and base pointer

	push eax	; Push multiboot magic number for identification

	; Push *virtual* multiboot pointer
	add ebx, 0xF8000000
	push ebx

	call init
	sti

.loop:
	hlt
	jmp .loop

section .ttext

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

global redo_paging
redo_paging :
	mov eax, [esp+4]
	lea ecx, [.lower-0xF8000000]
	jmp ecx	; Jump to lower memory copy of kernel

.lower:
	mov ecx, cr0
	and ecx, 0x7FFFFFFF	; Disable paging
	mov cr0, ecx

	mov ecx, cr4
	and ecx, 0xFFFFFFEF	; Disable 4 MB pages
	mov cr4, ecx

	mov cr3, eax		; Load new page directory

	mov ecx, cr0
	or  ecx, 0x80000000	; Re-enable paging
	mov cr0, ecx
	
	ret
