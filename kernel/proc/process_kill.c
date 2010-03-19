/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <mem.h>

/****************************************************************************
 * process_kill
 *
 * Frees a process entirely.
 */

void process_kill(process_t *proc) {
	sched_rem(proc->pid);

	process_free(proc);
}
