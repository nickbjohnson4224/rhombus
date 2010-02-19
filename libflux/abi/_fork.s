; Copyright 2010 Nick Johnson

[bits 32]

global _fork

_fork:
	int 0x66

	ret
