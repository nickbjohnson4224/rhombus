; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

global get_cr0
global set_cr0

get_cr0:
	mov eax, cr0
	ret

set_cr0:
	mov eax, [esp+4]
	mov cr0, eax
	ret

global clr_ts
global set_ts
global tst_ts

clr_ts:
	clts
	ret

set_ts:
	mov eax, cr0
	or eax, 0x8
	mov cr0, eax
	ret

tst_ts:
	mov eax, cr0
	and eax, 0x8
	ret
