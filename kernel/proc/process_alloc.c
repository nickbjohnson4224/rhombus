/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <task.h>

/****************************************************************************
 * process_alloc
 *
 * Returns a pointer to a process that was not previously allocated, and
 * sets the field 'pid' in that process structure. Returns null on error.
 */

process_t *process_alloc(void) {
	uintptr_t addr;
	process_t *proc;
	pid_t pid;

	for (pid = 0; pid < MAX_PID; pid++) {
		addr = PROCESS_TABLE + (PAGESZ * pid);

		if ((page_get(addr) & PF_PRES) == 0) {
			process_touch(pid);

			proc = (process_t*) addr;
			proc->pid = pid;

			return proc;
		}
	}

	return NULL;
}
