/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <space.h>
#include <irq.h>
#include <cpu.h>

/* TSS driver ***************************************************************/

/*****************************************************************************
 * _is_init
 *
 * Zero if set_int_stack has not been initialized, nonzero if it has.
 */

static int _is_init;

/*****************************************************************************
 * tss
 *
 * The "Task State Segment" also known as the TSS. This is part of the x86
 * architecture that can be used for hardware task switching, but we're only
 * using it to set the interrupt handler stack.
 */

static struct tss tss;

/*****************************************************************************
 * gdt
 *
 * The "Global Descriptor Table" also known as the GDT. This is the part of 
 * the x86 architecture that contains all of the protected mode segment 
 * descriptors. Each descriptor is actually 8 bytes in length, but we're
 * interpreting it as an array of bytes for simplicity. The TSS is the sixth
 * descriptor.
 *
 * The actual gdt is defined in "kernel/boot.s".
 */

extern uint8_t gdt[48];

/*****************************************************************************
 * int_stack_init
 *
 * Initializes the system responsible for set_int_stack(). On the x86, this
 * effectively means initializing the TSS, which is responsible for usermode
 * to kernelmode switches.
 */

static void int_stack_init() {
	uint32_t base = (uint32_t) &tss;
	uint16_t limit = (uint16_t) (base + sizeof(struct tss));

	memclr(&tss, sizeof(struct tss));
	tss.cs = 0x0B;
	tss.ss0 = tss.es = tss.ds = tss.fs = tss.gs = 0x10;

	/* Change the 6th GDT entry to be the TSS */
	gdt[40] = (uint8_t) ((limit) & 0xFF);
	gdt[41] = (uint8_t) ((limit >> 8) & 0xFF);
	gdt[42] = (uint8_t) (base & 0xFF);
	gdt[43] = (uint8_t) ((base >> 8) & 0xFF);
	gdt[44] = (uint8_t) ((base >> 16) & 0xFF);
	gdt[47] = (uint8_t) ((base >> 24) & 0xFF);

	cpu_sync_tss();
}

/*****************************************************************************
 * set_int_stack
 *
 * Sets the pointer at which the stack will start when an interrupt is
 * handled. This stack is where the thread state is saved, not the global
 * kernel stack: in general, it should always be set to &thread->kernel_stack 
 * for the currently running thread.
 */

void set_int_stack(void *ptr) {
	if (!_is_init) {
		int_stack_init();
		_is_init = 1;
	}

	tss.esp0 = (uintptr_t) ptr;
}
