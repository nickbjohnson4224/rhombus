; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

section .data

global fpu_p
fpu_p:
	dd 0

section .text

global fpu_init
fpu_init:
	push ebx

	; check CPUID for FXSR bit
	mov eax, 1
	cpuid
	test edx, 0x01000000
	jne .nofx

	; set OSFXSR bit in CR4
	mov eax, cr4
	or eax, 0x200
	mov cr4, eax

	mov eax, 1
	mov [fpu_p], eax

	; check CPUID for FPU bit
	test edx, 0x00000001
	jne .nofpu
	
	; set FPU control word
.cw:
	mov ax, 0x37F
	mov [.cw], ax
	fldcw [.cw]

.nofpu:
	; check CPUID for SSE bit
	test edx, 0x02000000
	jne .nosse

	; initialize SSE
	mov eax, cr0
	and eax, 0xFFFFFFFB
	or eax, 0x2
	mov cr0, eax
	mov eax, cr4
	or eax, 0x00000600
	mov cr4, eax

.nosse:
.nofx:
	pop ebx
	ret

