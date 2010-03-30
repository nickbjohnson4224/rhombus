/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <int.h>

/****************************************************************************
 * thread_free
 *
 * Frees a thread and its associated memory.
 */

void thread_free(thread_t *thread) {
	frame_free(page_ufmt(page_get((uintptr_t) thread)));
	page_set((uintptr_t) thread, 0);
}
