/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/proc.h>
#include <kernel/memory.h>

/****************************************************************************
 * thread_spawn
 *
 * This function creates a new thread in process "process" and returns it.
 * The thread has an unique thread identification number. The thread structure
 * is within the fixed segment SEGMENT_ADDR_THREAD in the process' address
 * space, at the page that is the lower 16 bits of its thread identification
 * number. The bits higher than the lower 16 bits are identical to those of
 * the process.
 *
 * It is guaranteed that when this function is called, the process "process"
 * is loaded on the calling processor.
 */

thread_t *thread_spawn(proc_t *process) {
	thread_t *thread;
	space_t space;
	frame_t frame;
	uint32_t id;
	void *stack;
	
	/* load process space */
	space = process->space;
	space_load(space);

	/* lock thread space, spinning */
	while (!mutex_aquire(&process->m_threads, 1));

	/* locate first empty thread space */
	for (id = 0; id < SEGMENT_PAGES; id++) {
		if (!page_check(SEGMENT_ADDR_THREAD + PAGESZ * id)) {
			thread = (thread_t*) (SEGMENT_ADDR_THREAD + PAGESZ * id);
			break;
		}
	}

	/* allocate thread structure memory */
	frame = frame_alloc();
	page_set(space, thread, frame | FRAME_READ | FRAME_WRITE | FRAME_SUPER);

	/* unlock thread space */
	mutex_release(&process->m_threads, 1);

	/* copy thread basics from thread 0 */
	memcpy(thread, process->threads, sizeof(thread_t));

	/* allocate stack segment */
	stack = seg_alloc(space, SEGMENT_TYPE_STACK);
	stack = (void*) ((uintptr_t) stack + SEGMENT_SIZE - THREAD_STACKSIZE);
	mem_alloc(stack, THREAD_STACKSIZE);
	thread->stack = (uintptr_t) stack + THREAD_STACKSIZE - sizeof(uintptr_t);

	/* set thread metadata */
	thread->ctrl.id     = id | (process->ctrl.id << 16);
	thread->ctrl.m_core = MUTEX_OPEN;

	return thread;
}
