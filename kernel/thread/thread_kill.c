/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/proc.h>
#include <kernel/memory.h>

/****************************************************************************
 * thread_kill
 *
 * This function destroys a thread and frees its stack and thread structure.
 */

void thread_kill(thread_t *thread) {
	space_t space;
	frame_t frame;
	proc_t *process;

	process = thread->proc;
	space = process->space;

	/* free stack segment */
	seg_free(thread->stack);

	/* lock thread space, spin */
	while (!mutex_aquire(thread->proc->m_threads, 1));

	/* free thread structure */
	frame = page_get(space, thread);
	frame_free(frame);
	page_set(space, thread, 0);

	/* unlock thread space */
	mutex_release(thread->proc->m_threads, 1);
}
