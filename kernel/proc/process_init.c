/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <task.h>

/****************************************************************************
 * process_init
 *
 * Initializes process subsystem.
 */

void process_init() {
	extern uint32_t get_cr3(void);
	process_t *idle;

	/* bootstrap process 0 (idle) */
	idle = process_alloc();
	idle->space = get_cr3();
	idle->flags = CTRL_READY | CTRL_SUPER;
	idle->sigflags = 0;

	/* fork process 1 and switch */
	process_switch(process_clone(idle), 0);
}
