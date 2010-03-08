/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/proc.h>
#include <kernel/memory.h>

/****************************************************************************
 * thread_switch
 *
 * This function makes sure that the given thread is active on this processor.
 * If the process owning the current thread is not the process owning the
 * thread to be switched to, the process is also switched. If the thread that
 * is to be switched to is locked by another processor, the function does not
 * switch threads and returns the old thread instead. On success, the function
 * returns the thread that has been switched to.
 */

thread_t *thread_switch(thread_t *thread, thread_t *old) {

	/* save old thread FPU state */
	fpu_save(old->fxdata);

	/* release lock on old thread */
	mutex_release(&old->ctrl.m_core, 1);

	/* switch to process */
	space_load(thread->proc->space);
	
	/* lock new thread; fail back otherwise */
	if (!mutex_aquire(&thread->ctrl.m_core, 1)) {

		/* switch back */
		if (old->proc != thread->proc) {
			space_load(thread->proc->space);
		}

		/* re-lock old thread, spinning if necessary */
		while (!mutex_aquire(&old->ctrlspace.ctrl.m_core, 1));

		return old;

	}

	/* load new FPU state */
	fpu_load(thread->fxdata);

	return thread;
}
