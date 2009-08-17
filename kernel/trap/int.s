; Copyright 2009 Nick Johnson

section .data
extern idt
idt_ptr:
	dw 0x2FF
	dd idt

section .text
align 4

global idt_flush
idt_flush:
	mov eax, idt_ptr
	lidt [eax]
	ret

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
INTN	64 ; fork
INTN	65 ; exit
INTN	66 ; sint
INTN	67 ; sret
INTN	68 ; mmap
INTN	69 ; umap
INTN	70 ; rsig
INTN	71 ; lsig

INTN	80 ; rirq
INTN	81 ; lirq
INTN	82 ; push
INTN	83 ; pull
INTN	84 ; eout

extern int_handler

int_common:
	pusha

	mov eax, 0
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov ebp, esp
	
	extern stack
	mov esp, (stack + 0x2000)	; Setup stack

	push ebp
	call int_handler
	mov esp, eax

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	add esp, 12
	sti
	iret
