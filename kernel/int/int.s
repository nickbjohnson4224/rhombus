; Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
; 
; Permission to use, copy, modify, and distribute this software for any
; purpose with or without fee is hereby granted, provided that the above
; copyright notice and this permission notice appear in all copies.
; 
; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#define ASM
#include <rho/arch.h>

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

INTN 	0	; #DE - Divide Error
INTN 	1
INTN 	2	; Non-Maskable Interrupt
INTN 	3	; #BP - Breakpoint
INTN 	4	; #OF - Overflow
INTN 	5	; #BR - Bound Range Exceeded
INTN	6	; #UD - Invalid Opcode
INTN 	7	; #NM - No Math Coprocessor
INTE	8	; #DF - Double Fault
INTN	9	; Coprocessor Segment Overrun
INTE	10	; #TS - Invalid TSS
INTE	11	; #NP - Segment Not Present
INTE	12	; #SS - Stack Segment Fault
INTE	13	; #GP - General Protection Fault
INTE	14	; #PF - Page Fault
INTN 	15
INTN 	16	; #MF - Math Fault
INTN 	17	; #AC - Alignment Check
INTN 	18	; #MC - Machine Check
INTN	19	; #XM - SIMD Exception

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
INTN	64	; send
INTN	65	; done
INTN	66	; when
INTN	67	; rirq
INTN	68	; also
INTN	69	; stat
INTN	70  ; page
INTN	71	; phys
INTN	72	; fork
INTN	73	; exit
INTN	74	; stop
INTN	75	; wake
INTN	76	; gpid
INTN	77	; time
INTN	78	; user -- deprecated
INTN	79	; auth -- deprecated
INTN	80	; proc
INTN	81	; kill
INTN	82	; vm86
INTN	83	; name

INTN	85	; reap

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
