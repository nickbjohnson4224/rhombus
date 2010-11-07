; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

; ***************************************************************************
; page_flush
;
; Makes sure all paging caches are synchronized for a given page.

global page_flush

page_flush:
	mov eax, [esp+4]
	invlpg [eax]
	ret
