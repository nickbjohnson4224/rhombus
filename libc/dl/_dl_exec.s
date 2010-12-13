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

section .text

global _dl_exec:function _dl_exec.end-_dl_exec
extern _dl_entry
extern _dl_start
extern _dl_end
extern _page

_dl_exec:

	; copy dynamic linker to 0xC0000000
	mov eax, _dl_start
	mov ecx, _dl_end
	sub ecx, _dl_start
	shr ecx, 12
	inc ecx
	
	push eax
	push dword 4
	push dword 0
	push ecx
	push 0xC0000000
	call _page
	add  esp, 20

	; load list pointer into EAX
	mov eax, [esp+4]

	; load entry pointer into ECX and relocate
	mov ecx, _dl_entry
	sub ecx, _dl_start
	add ecx, 0xC0000000

	push eax
	call ecx
	add  esp, 4

	ret
.end:
