/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <mem.h>

/****************************************************************************
 * process_clone
 *
 * Copies a process entirely, returning a pointer to the new process 
 * structure. The new process is independent of the old, and is referred to
 * as the 'child'.
 */

process_t *process_clone(process_t *parent) {
	process_t *child;
	pid_t pid;

	child = process_alloc();
	pid   = child->pid;

	memcpy(child, parent, sizeof(process_t));

	child->space  = space_clone();
	child->parent = parent->pid;
	child->pid    = pid;

	sched_ins(pid);

	return child;
}
