; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

; ***************************************************************************
; mutex_aquire
;
; This function aquires a 32 bit mutex in a thread-safe fashion. If the mutex
; is already aquired, then the mutex is not aquired, and the function returns
; false. On success, the function returns true, and the mutex contains the
; given key.

; bool mutex_aquire(uint32_t *mutex, uint32_t key);
global mutex_aquire
mutex_aquire:
	push ebx
	push edx

	mov ebx, [esp+12] ; uint32_t *mutex
	mov edx, [esp+16] ; uint32_t key

	xor eax, eax
	lock cmpxchg [ebx], edx
	jnz .fail

	mov eax, 1
	.fail:
	pop edx
	pop ebx
	ret
