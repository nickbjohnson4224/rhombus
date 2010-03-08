; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

#define ASM
#include <flux/config.h>

%macro INTN 1
  global int%1
  int%1:
    cli
	sub esp, 4	; Reserve space for caller
    push byte 0
    push byte %1
    jmp int_common
%endmacro

%macro INTE 1
  global int%1
  int%1:
    cli
	sub esp, 4	; Reserve space for caller
    push byte %1
    jmp int_common
%endmacro 

INTN 	0
INTN 	1
INTN 	2
INTN 	3
INTN 	4
INTN 	5
INTN	6
INTN 	7
INTE	8
INTN	9
INTE	10
INTE	11
INTE	12
INTE	13
INTE	14
INTN 	15
INTN 	16
INTN 	17
INTN 	18

; IRQs
INTN	32
INTN	33
INTN	34
INTN	35
INTN	36
INTN	37
INTN	38
INTN	39
INTN	40
INTN	41
INTN	42
INTN	43
INTN	44
INTN	45
INTN	46
INTN	47

; System calls
INTN	96  ; fire
INTN	97  ; drop
INTN	98  ; hand
INTN	99  ; ctrl
INTN	100 ; info
INTN	101 ; mmap
INTN	102 ; fork
INTN	103 ; exit

extern int_handler
extern fpu_save
extern fpu_load
global int_return

int_common:
	pusha

	mov eax, 0
	mov ax, ds
	push eax
	push 0x42242442
	sub esp, 520

	mov cx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx

	mov ebp, esp

	; Setup stack
	extern kstack
 
	mov esp, (kstack + 0x1FF0)

	push ebp
	call int_handler
	mov esp, eax

int_return:
	add esp, 524
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	add esp, 12
	iret
