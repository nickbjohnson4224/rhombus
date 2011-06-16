; Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
; 
; Permission to use, copy, modify, and distribute this software for any
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

section .text

global _name:function _name.end-_name

_name:
	push ebx
	push esi
	push edi

	mov edi, [esp+24]

	cmp edi, 0
	jne .send
	jmp .recv

.send:
	; load string
	mov esi, [esp+16]
	mov eax, [esi+0]
	mov ebx, [esi+4]
	mov ecx, [esi+8]
	mov edx, [esi+12]

	mov esi, edi
	mov edi, [esp+20]

	int 0x53
	jmp .cleanup

.recv:
	mov esi, edi
	mov edi, [esp+20]

	int 0x53

	; store string
	mov esi, [esp+16]
	mov [esi+0], eax
	mov [esi+4], ebx
	mov [esi+8], ecx
	mov [esi+12], edx

.cleanup:
	pop edi
	pop esi
	pop ebx

	ret
.end:
