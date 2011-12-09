[bits 32]

section .text

global __plt_resolve:function __plt_resolve.end-__plt_resolve
extern _plt_resolve

__plt_resolve:
	call _plt_resolve
	add esp, 8
	jmp eax

.end:
