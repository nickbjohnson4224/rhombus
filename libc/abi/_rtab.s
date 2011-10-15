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

global _rtab:function _rtab.end-_rtab

_rtab:
	push ebx
	push esi
	push edi

	mov ebx, [esp+16]
	mov ecx, [esp+20]
	mov edx, [esp+24]
	mov esi, [esp+28]
	mov edi, [esp+32]

	int 0x54

	pop edi
	pop esi
	pop ebx
	ret
.end:
