; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

; ***************************************************************************
; page_flush_full
;
; Makes sure all paging caches are synchronized.

global page_flush_full

page_flush_full:
	mov eax, cr3
	mov cr3, eax
	ret
