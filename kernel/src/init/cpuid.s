; Copyright 2009 Nick Johnson

section .ttext

global cpuid_name
cpuid_name:
	mov eax, 0
	mov edi, [esp+4]
	cpuid
	mov [edi+0], ebx
	mov [edi+4], edx
	mov [edi+8], ecx
	ret

