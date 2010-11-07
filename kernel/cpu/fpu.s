; Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
; 
; Permission to use, copy, modify, and/or distribute this software for any
; purpose with or without fee is hereby granted, provided that the above
; copyright notice and this permission notice appear in all copies.
; 
; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

[bits 32]

section .data

global can_use_fpu

can_use_fpu: 
	dd 0

section .text

global cpu_init_fpu
global fpu_save
global fpu_load

cpu_init_fpu:
	push ebx

	mov eax, 1
	mov [can_use_fpu], eax

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
