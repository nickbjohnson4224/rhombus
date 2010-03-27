/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <int.h>
#include <task.h>

/****************************************************************************
 * thread_switch
 *
 * Switches to the target thread, completely saving the old threads' state
 * and loading the new ones' state. A process switch is performed if and only
 * if the threads are under different processes. A pointer to the switched to
 * thread is returned.
 */

thread_t *thread_switch(thread_t *old, thread_t *new) {
	extern void fpu_save(void *fxdata);
	extern void fpu_load(void *fxdata);

	/* save FPU state */
	if (old->proc->flags & CTRL_FLOAT) {
		fpu_save(old->fxdata);
	}

	/* switch processes */
	if (old->proc != new->proc) {
		process_switch(new->proc, 0);
	}

	/* load FPU state */
	if (new->proc->flags & CTRL_FLOAT) {
		fpu_load(new->fxdata);
	}

	return new;
}
