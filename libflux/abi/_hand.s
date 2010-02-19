; Copyright 2010 Nick Johnson

[bits 32]

global _hand

_hand:
	mov eax, [esp+4]
	int 0x62

	ret
