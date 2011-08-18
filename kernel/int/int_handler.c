/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <interrupt.h>
#include <string.h>
#include <space.h>
#include <irq.h>
#include <cpu.h>

/*****************************************************************************
 * _is_init
 *
 * Set to zero if the interrupt system has not been initialized, nonzero if
 * it has.
 */

static int _is_init;
static void init_int_handler(void);

/* interrupt handling *******************************************************/

/*****************************************************************************
 * _int_handler
 *
 * Array of registered interrupt handlers. Modified exclusively by 
 * int_set_handler(), and accessed exclusively by int_handler().
 */

static int_handler_t _int_handler[256];

/*****************************************************************************
 * int_set_handler
 *
 * Sets the interrupt with id <n> to call <handler> when fired.
 */

void int_set_handler(intid_t n, int_handler_t handler) {

	if (!_is_init) {
		init_int_handler();
		_is_init = 1;
	}

	_int_handler[n] = handler;
}

/*****************************************************************************
 * int_handler
 *
 * Handles all interrupts, redirecting them to the proper interrupt handlers.
 * This function is only called by the state-saving assembly routine in
 * "int.s", which is why it's not in any header files but non-static.
 */

struct thread *int_handler(struct thread *image) {
	struct thread *new_image;

	/* reset IRQs if it was an IRQ */
	if (ISIRQ(image->num)) {
		irq_reset(INT2IRQ(image->num));
	}

	/* call registered interrupt handler */
	if (_int_handler[image->num]) {
		new_image = _int_handler[image->num](image);

		if (new_image && new_image != image) {
			image = thread_switch(image, new_image);
		}
	}

	/* set IOPL=3 if root, IOPL=0 if other user or vm86 */
	if ((image->user == 0) && (image->vm86_active == 0)) {
		image->eflags |= 0x3000;
	}
	else {
		image->eflags &= ~0x3000;
	}

	/* set or unset VM86 flag */
	if (image->vm86_active) {
		image->eflags |= 0x20000;
	}
	else {
		image->eflags &= ~0x20000;
	}

	image->eflags |= 0x200;
	return image;
}

/* IDT driver ***************************************************************/

/*****************************************************************************
 * idt
 *
 * The "Interrupt Descriptor Table" also known as the IDT. This is the part
 * of the x86 architecture that redirects interrupts to interrupt handlers.
 * These are the low-level assembly handlers (NOT the C handlers registered by
 * int_set_handler()).
 */

static struct idt idt[256];

/* Assembly interrupt handler stubs to be registered in the IDT *************/

extern void
	int0(void),  int1(void),  int2(void),  int3(void),
	int4(void),  int5(void),  int6(void),  int7(void),  
	int8(void),  int9(void),  int10(void), int11(void), 
	int12(void), int13(void), int14(void), int15(void), 
	int16(void), int17(void), int18(void), int19(void),

	int32(void), int33(void), int34(void), int35(void), 
	int36(void), int37(void), int38(void), int39(void), 
	int40(void), int41(void), int42(void), int43(void), 
	int44(void), int45(void), int46(void), int47(void),
	
	int64(void), int65(void), int66(void), int67(void), 
	int68(void), int69(void), int70(void), int71(void),
	int72(void), int73(void), int74(void), int75(void), 
	int76(void), int77(void), int78(void), int79(void),
	int80(void), int81(void), int82(void), int83(void),
	int84(void);

/*****************************************************************************
 * idt_raw
 *
 * Array of low-level interrupt handlers to be mirrored by the IDT. This
 * array is used to generate the IDT when init_int_handler() is called, and
 * is never used again.
 */

typedef void (*int_raw_handler_t) (void);
static int_raw_handler_t idt_raw[256] = {

	/* faults */
	int0, 	int1, 	int2, 	int3, 	int4, 	int5, 	int6, 	int7, 
	int8, 	int9, 	int10, 	int11, 	int12, 	int13, 	int14, 	int15, 
	int16,	int17, 	int18, 	int19, 	NULL, 	NULL, 	NULL, 	NULL, 
	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

	/* IRQs */
	int32,	int33, 	int34, 	int35, 	int36, 	int37, 	int38, 	int39, 
	int40,	int41, 	int42, 	int43, 	int44, 	int45, 	int46, 	int47, 

	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

	/* system calls */
	int64, 	int65, 	int66, 	int67, 	int68, 	int69, 	int70, 	int71, 
	int72, 	int73, 	int74, 	int75, 	int76, 	int77, 	int78, 	int79, 
	int80,	int81, 	int82, 	int83, 	int84, 	NULL, 	NULL, 	NULL, 
	NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
};

/****************************************************************************
 * idt_set
 *
 * Set the interrupt descriptor <n> to handler <base> in code segment <seg>
 * with flags <flags>. See the Intel Manuals for details on this.
 */

static void idt_set(intid_t n, uint32_t base, uint16_t seg, uint8_t flags) {

	if (!base) {
		return; /* Ignore null handlers */
	}
	
	idt[n].base_l = (uint16_t) (base & 0xFFFF);
	idt[n].base_h = (uint16_t) (base >> 16);
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

/****************************************************************************
 * init_int_handler
 *
 * Initialize the interrupt handling system.
 */

static void init_int_handler(void) {
	size_t i;

	/* Write privileged interrupt handlers (faults, IRQs) */
	for (i = 0; i < 64; i++) {
		if (idt_raw[i]) {
			idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
		}
	}

	/* Write usermode interrupt handlers (syscalls) */
	for (i = 64; i < 256; i++) {
		if (idt_raw[i]) {
			idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
		}
	}

	/* Write the IDT */
	cpu_set_idt(idt);
}
