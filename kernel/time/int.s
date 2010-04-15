; Copyright 2009 Nick Johnson
; ISC Licensed, see LICENSE for details

#define ASM
#include <flux/config.h>

%macro INTN 1
  global int%1
  int%1:
    cli
    push byte 0
    push byte %1
    jmp int_common
%endmacro

%macro INTE 1
  global int%1
  int%1:
    cli
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
INTN	64	; fire
INTN	65	; drop
INTN	66	; sctl
INTN	67	; mail

INTN	72	; fork
INTN	73	; exit
INTN	74	; pctl
INTN	75	; kctl

INTN	80	; mmap
INTN	81	; mctl

extern int_handler
extern fpu_save
extern fpu_load
global int_return

int_common:
	pusha

	xor eax, eax
	mov ax, ds
	push eax

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
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	add esp, 8
	iret
