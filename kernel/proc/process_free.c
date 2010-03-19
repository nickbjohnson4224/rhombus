/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <task.h>

/****************************************************************************
 * process_free
 *
 * Frees the memory associated with a process structure. This does not
 * include threads: use process_kill() for more complete freeing.
 */

void process_free(process_t *proc) {
	uintptr_t addr;

	addr = (uintptr_t) proc;

	if (addr < PROCESS_TABLE || addr >= PROCESS_TABLE + (PAGESZ * MAX_PID)) {
		return;
	}

	frame_free(page_get(addr));
	page_set(addr, 0);
}
