/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <time.h>
#include <space.h>

/****************************************************************************
 * process_table
 *
 * Array of pointers to process structures used to quickly associate 
 * processes with their corresponding PIDs and otherwise keep track of them.
 */

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

	/* allocate new process structure for child */
	child = process_alloc();
	pid   = child->pid;

	/* copy parent */
	memcpy(child, parent, sizeof(process_t));

	/* setup child */
	child->space  = space_clone();
	child->parent = parent;
	child->pid    = pid;
	child->image  = thread_alloc();

	/* copy parent thread */
	if (parent->image) {
		memcpy(child->image, parent->image, sizeof(thread_t));

		/* copy parent FPU/SSE state */
		if (parent->image->fxdata) {
			child->image->fxdata = heap_alloc(512);
			memcpy(child->image->fxdata, parent->image->fxdata, 512);
		}
	}

	/* setup child thread */
	child->image->tis  = NULL;
	child->image->proc = child;

	/* add child to scheduler */
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
	else {
		return process_table[pid];
	}
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
	idle->flags = CTRL_READY | CTRL_SUPER | CTRL_QUEUE;

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
	space_load(proc->space);

	return proc->image;
}
