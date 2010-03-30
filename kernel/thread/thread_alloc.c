/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <int.h>

/****************************************************************************
 * thread_alloc
 *
 * Returns a pointer to a thread that was not previously allocated. Returns
 * null on error. The thread structure is page aligned.
 */

thread_t *thread_alloc(void) {
	uintptr_t addr;
	thread_t *thread;
	size_t i;

	for (i = 0; i < 4096; i++) {
		addr = THREAD_TABLE + (PAGESZ * i);

		if ((page_get(addr) & PF_PRES) == 0) {
			page_set(addr, page_fmt(frame_new(), PF_PRES | PF_RW));

			thread = (thread_t*) addr;

			return thread;
		}
	}

	return NULL;
}
