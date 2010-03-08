; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

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
