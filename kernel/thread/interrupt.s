; Copyright 2010 Nick Johnson
; ISC Licensed, see LICENSE for details

extern thread_save

%macro INTN 1
  global int%1
  int%1:
    cli
    push byte 0
    push byte %1
    jmp thread_save
%endmacro

%macro INTE 1
  global int%1
  int%1:
    cli
    push byte %1
    jmp thread_save
%endmacro 

; Exceptions
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

; System calls (ABI 4)
INTN	128 ; fire
INTN	129 ; ctrl
INTN	130 ; mmap
INTN	131 ; load
INTN	132 ; fork
INTN	133 ; exit
