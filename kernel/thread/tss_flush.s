; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global tss_flush
tss_flush:
	mov ax, 0x28
	ltr ax
	ret
