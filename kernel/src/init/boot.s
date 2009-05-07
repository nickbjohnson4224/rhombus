; Copyright 2009 Nick Johnson

; Multiboot stuff
MODULEALIGN equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ  -(MAGIC + FLAGS)

section .bss

STACKSIZE equ 0x1000
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

gdt:
align 0x1000
	dd 0x00000000, 0x00000000
	dd 0x0000FFFF, 0x00CF9A00
	dd 0x0000FFFF, 0x00CF9200
	dd 0x0000FFFF, 0x00CFFA00
	dd 0x0000FFFF, 0x00CFF200
	dd 0x00000000, 0x0000E900

gdt_ptr:
align 4
	dw 0x0027	; 39 bytes limit
	dd gdt 		; Points to physical GDT

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
	mov ecx, 0x10		; Load all kernel data segments
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx
	mov esp, (init_stack + STACKSIZE)	; Setup init stack
	mov ebp, (init_stack + STACKSIZE)	; and base pointer
	push eax	; Push multiboot magic number
	add ebx, 0xF8000000	; Make pointer virtual
	push ebx	; Push multiboot pointer

	call init
	hlt			; Halt for now 
	
global usermode
usermode:
	; change to user data segments
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; push user stack segment
	push 0x23

	; push user stack pointer
	mov eax, 0xF0000000
	push eax

	; push EFLAGS (and turn on interrupts)
	pushf
	pop eax
	or eax, 0x200
	push eax

	; push user code segment
	mov eax, 0x1B
	push eax

	; push user start location
	lea eax, [init]
	push eax

	iret

section .ttext

global redo_paging
redo_paging :
	mov eax, [esp+4]
	lea ecx, [.lower-0xF8000000]
	jmp ecx

.lower:
	mov ecx, cr0
	and ecx, 0x7FFFFFFF
	mov cr0, ecx
	mov ecx, cr4
	and ecx, 0xFFFFFFEF
	mov cr4, ecx
	mov cr3, eax
	mov ecx, cr0
	or  ecx, 0x80000000
	mov cr0, ecx
	ret
