/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <int.h>
#include <init.h>

/****************************************************************************
 * thread_init
 *
 * Initializes threading subsystem.
 */

void thread_init(void) {
	uint16_t divisor;
	thread_t *image;

	/* set up interrupt descriptor table */
	init_idt();

	/* initialize task state segment */
	init_tss();

	/* set programmable interrupt controller mask */
	pic_mask(0x0001);

	/* register system calls */
	register_int(0x60, fire);
	register_int(0x61, drop);
	register_int(0x62, hand);
	register_int(0x63, ctrl);
	register_int(0x64, info);
	register_int(0x65, mmap);
	register_int(0x66, fork);
	register_int(0x67, exit);

	/* register fault handlers */
	register_int(0x00, fault_float);
	register_int(0x01, fault_generic);
	register_int(0x02, fault_generic);
	register_int(0x03, fault_generic);
	register_int(0x04, fault_generic);
	register_int(0x05, fault_generic);
	register_int(0x06, fault_generic);
	register_int(0x07, fault_float);
	register_int(0x08, fault_double);
	register_int(0x09, fault_float);
	register_int(0x0A, fault_generic);
	register_int(0x0B, fault_generic);
	register_int(0x0C, fault_generic);
	register_int(0x0D, fault_generic);
	register_int(0x0E, fault_page);
	register_int(0x0F, fault_generic);
	register_int(0x10, fault_float);
	register_int(0x11, fault_generic);
	register_int(0x12, fault_generic);

	/* initialize programmable interrupt timer at 256Hz */
	register_int(IRQ(0), pit_handler);
	divisor = 1193180 / 256;
	outb(0x43, 0x36);
	outb(0x40, (uint8_t) (divisor & 0xFF));
	outb(0x40, (uint8_t) (divisor >> 8));

	/* initialize FPU/MMX/SSE */
	init_fpu();

}
