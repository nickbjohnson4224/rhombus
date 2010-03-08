/*
 * Copyright 2010 Nick Johnson
 * ISC Licnensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/io.h>

/****************************************************************************
 * int_reg_handler
 *
 * Array of function pointers used to handle interrupts with high level C
 * functions.
 */

int_handler_t int_reg_handler[256];

/****************************************************************************
 * int_handler
 *
 * This function is directly called by the assembly function thread_save after
 * interrupts. It redirects the thread image produced by thread_save to the
 * proper C function registered in int_reg_handler[].
 */

thread_t *int_handler(thread_t *state) {

	/* Reset PIC if int was an IRQ */
	if (image->num >= 32 && image->num <= 47) {
		if (image->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	/* Call registered C interrupt handler from int_reg_handler[] table */
	if (int_handler[state->num]) {
		state = int_handler[state->num](state);
	}

	/* Set tss to (potentially) new thread */
	tss_set_esp((uint32_t) &state->tss_start);

	return state;
}
