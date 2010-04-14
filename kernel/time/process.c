/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <task.h>

process_t *process_table[MAX_TASKS];

/****************************************************************************
 * process_alloc
 *
 * Returns a pointer to a process that was not previously allocated, and
 * sets the field 'pid' in that process structure. Returns null on error.
 */

process_t *process_alloc(void) {
	uintptr_t i;

	for (i = 0; i < MAX_TASKS; i++) {
		if (process_table[i] == NULL) {
			break;
		}
	}

	process_table[i] = heap_alloc(sizeof(process_t));
	process_table[i]->pid = i;
	return process_table[i];
}

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
	child->image  = thread_alloc();

	if (parent->image) {
		memcpy(child->image, parent->image, sizeof(thread_t));
	}

	child->image->tis  = NULL;
	child->image->proc = child;

	sched_ins(pid);

	return child;
}

/****************************************************************************
 * process_free
 *
 * Frees the memory associated with a process structure. This does not
 * include threads: use process_kill() for more complete freeing.
 */

void process_free(process_t *proc) {
	uintptr_t i;

	for (i = 0; i < MAX_PID; i++) {
		if (process_table[i] == proc) {
			break;
		}
	}

	process_table[i] = NULL;
	heap_free(proc, sizeof(process_t));
}

/****************************************************************************
 * process_get
 *
 * returns a pointer to the process with the specified pid if the process
 * exists, and null otherwise.
 */

process_t *process_get(pid_t pid) {

	if (pid >= MAX_TASKS) {
		return NULL;
	}

	return process_table[pid];
}

/****************************************************************************
 * process_init
 *
 * Initializes process subsystem.
 */

void process_init() {
	extern uint32_t get_cr3(void);
	process_t *idle;

	/* bootstrap process 0 (idle) */
	idle = process_alloc();
	idle->space = get_cr3();
	idle->flags = CTRL_READY | CTRL_SUPER;
	idle->sigflags = 0;

	/* fork process 1 and switch */
	process_switch(process_clone(idle), 0);
}

/****************************************************************************
 * process_kill
 *
 * Frees a process entirely.
 */

void process_kill(process_t *proc) {
	sched_rem(proc->pid);

	process_free(proc);
}

/****************************************************************************
 * process_switch
 *
 * Switches to the target process and its specified thread.
 */

struct thread *process_switch(struct process *proc, uint32_t thread) {

	/* switch task */
	curr_task = proc;
	curr_pid = proc->pid;
	space_load(proc->space);

	return proc->image;
}
