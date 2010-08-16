; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

; FPU/SSE init/save/load functions

[bits 32]

section .data

global can_use_fpu

can_use_fpu: 
	dd 0

section .text

global init_fpu
global fpu_save
global fpu_load

init_fpu:
	push ebx

	mov eax, 1
	mov [can_use_fpu], eax

;	; check CPUID for FPU bit
;	test edx, 0x00000001
;	jnz .nofpu

	; initialize FPU
	finit
	mov eax, cr0
	and eax, 0xFFFFFFFB	; clear EM
	or eax,  0x00000022 ; set MP, NS
	mov cr0, eax

.nofpu:
	pop ebx
	ret

fpu_save:
	mov eax, [can_use_fpu]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	fsave [ecx]
.blank:
	ret

fpu_load:
	mov eax, [can_use_fpu]
	cmp eax, 0
	je .blank
	mov ecx, [esp+4]
	frstor [ecx]
.blank:
	ret
