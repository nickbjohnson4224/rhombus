; Copyright 2009 Nick Johnson

; Multiboot stuff
MODULEALIGN equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MODULEALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ  -(MAGIC + FLAGS)

section .bss

TSTACKSIZE equ 0xF0
global tstack
align 4
tstack:
	resb 0x100

section .bss

KSTACKSIZE equ 0x1FF0
global kstack
kstack:
	resb 0x2000

section .pdata

; Initial kernel address space
init_kmap:
    dd (init_ktbl - 0xFE000000 + 3)	
	times 1015 dd 0	; Fill until 0xFE000000
	dd (init_ktbl - 0xFE000000 + 3)
	times 6 dd 0	; Fill remainder of map
	dd (init_kmap - 0xFE000000 + 3)

init_ktbl:
%assign i 0
%rep 	1024
		dd (i << 12) | 3
%assign i i+1
%endrep

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

section .mboot

; Multiboot header
MultiBootHeader:
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .text

extern init
extern int_return
global start
start:
	cli
	mov ecx, init_kmap - 0xFE000000	; Physical address
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

	; Clear unneeded lomem identity map
	mov edx, init_kmap
	xor ecx, ecx
	mov [edx], ecx

	mov esp, (kstack + KSTACKSIZE)	; Setup init stack
	mov ebp, (kstack + KSTACKSIZE)	; and base pointer

	push eax	; Push multiboot magic number for identification

	; Push *virtual* multiboot pointer
	add ebx, 0xFE000000
	push ebx

	call init

	sti
	jmp $
