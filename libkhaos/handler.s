extern sret_call
extern khsignal_redirect
; void khsignal_redirect(uint32_t source, uint8_t signal, uint32_t args[4]);

global khsignal_handler
khsignal_handler:

	push edx
	push ecx
	push ebx
	push eax
	push esp
	push edi
	push esi
	call khsignal_redirect
	add esp, 28

	mov eax, 3
	push eax
	call sret_call
