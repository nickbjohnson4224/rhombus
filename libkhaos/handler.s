extern sret_call
extern khsig_redirect
; void khsig_redirect(uint32_t source, uint8_t signal, uint32_t args[4]);

global khsig_handler
khsig_handler:

	push edx
	push ecx
	push ebx
	push eax
	push esp
	push edi
	push esi
	call khsig_redirect
	add esp, 28

	mov eax, 3
	push eax
	call sret_call
