; Copyright 2010 Nick Johnson

[bits 32]

global _info

_info:
	mov eax, [esp+4]
	int 0x64

	ret
