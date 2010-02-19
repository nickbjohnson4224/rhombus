; Copyright 2010 Nick Johnson

[bits 32]

global _ctrl

_ctrl:
	mov eax, [esp+4]
	mov edx, [esp+8]
	mov ecx, [esp+12]
	int 0x63

	ret
