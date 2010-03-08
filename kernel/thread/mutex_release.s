; Copyright 2010 Nick Johnson
; ISC Licnensed, see LICENSE for details

; ***************************************************************************
; mutex_release
; 
; This function releases a 32 bit mutex in a thread-safe fashion. If the value
; "key" does not match the mutex, the mutex is not released, and the function
; returns false. On success, the function returns true, and the mutex contains 
; the value 0.

; bool mutex_release(uint32_t *mutex, uint32_t key);
global mutex_release
mutex_release:
	push ebx
	push edx

	mov ebx, [esp+12] ; uint32_t *mutex
	mov eax, [esp+16] ; uint32_t key

	xor edx, edx
	lock cmpxchg [ebx], edx
	jnz .fail
	
	mov eax, 1
	jmp .end
	.fail:
	xor eax, eax
	.end:
	pop edx
	pop ebx
	ret
