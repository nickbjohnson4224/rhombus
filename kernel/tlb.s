[bits 32]

global page_flush_full
page_flush_full:
	mov eax, cr3
	mov cr3, eax
	ret

global page_flush
page_flush:
	mov eax, [esp+4]
	invlpg [eax]
	ret

global map_load
map_load:
	mov eax, [esp+4]
	mov cr3, eax
	ret
