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

struct process *process_table[MAX_TASKS];

/****************************************************************************
 * process_alloc
 *
 * Returns a pointer to a process that was not previously allocated, and
 * sets the field 'pid' in that process structure. Returns null on error.
 */

struct process *process_alloc(void) {
	uintptr_t i;

	for (i = 0; i < MAX_TASKS; i++) {
		if (process_table[i] == NULL) {
			break;
		}
	}

	process_table[i] = heap_alloc(sizeof(struct process));
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

struct process *process_clone(struct process *parent, struct thread *active) {
	struct process *child;
	struct thread *new_thread;
	uint32_t pid, i;

	/* allocate new process structure for child */
	child = process_alloc();
	pid   = child->pid;

	/* copy parent */
	memcpy(child, parent, sizeof(struct process));

	/* setup child */
	child->space  = space_clone();
	child->parent = parent;
	child->pid    = pid;

	memclr(child->thread, sizeof(struct thread*) * 256);

	new_thread = thread_alloc();

	/* copy parent thread */
	if (active) {
		memcpy(new_thread, active, sizeof(struct thread));

		/* copy parent FPU/SSE state */
		if (active->fxdata) {
			new_thread->fxdata = heap_alloc(512);
			memcpy(new_thread->fxdata, active->fxdata, 512);
		}
	}

	/* setup child thread */
	thread_bind(new_thread, child);

	/* add child's thread to the scheduler */
	schedule_insert(new_thread);

	return child;
}

/****************************************************************************
 * process_free
 *
 * Frees the memory associated with a process structure. This does not
 * include threads: use process_kill() for more complete freeing.
 */

void process_free(struct process *proc) {
	uintptr_t i;

	for (i = 0; i < MAX_PID; i++) {
		if (process_table[i] == proc) {
			break;
		}
	}

	process_table[i] = NULL;
	heap_free(proc, sizeof(struct process));
}

/****************************************************************************
 * process_get
 *
 * returns a pointer to the process with the specified pid if the process
 * exists, and null otherwise.
 */

struct process *process_get(uint32_t pid) {

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
	struct process *idle;

	/* bootstrap process 0 (idle) */
	idle = process_alloc();
	idle->space = get_cr3();
	idle->flags = CTRL_READY | CTRL_SUPER;

	/* fork process 1 and switch */
	process_switch(process_clone(idle, NULL));
}

/****************************************************************************
 * process_freeze
 *
 * Freeze all threads in a process once.
 */

void process_freeze(struct process *proc) {
	size_t i;

	for (i = 0; i < 256; i++) {
		if (proc->thread[i]) {
			thread_freeze(proc->thread[i]);
		}
	}
}

/****************************************************************************
 * process_thaw
 *
 * Thaw all threads in a process once.
 */

void process_thaw(struct process *proc) {
	size_t i;

	for (i = 0; i < 256; i++) {
		if (proc->thread[i]) {
			thread_thaw(proc->thread[i]);
		}
	}
}

/****************************************************************************
 * process_kill
 *
 * Frees a process entirely.
 */

void process_kill(struct process *proc) {
	size_t i;

	for (i = 0; i < 256; i++) {
		if (proc->thread[i]) {
			thread_free(proc->thread[i]);
		}
	}

	space_free(proc->space);
	process_free(proc);
}

/****************************************************************************
 * process_switch
 *
 * Switches to the target process.
 */

void process_switch(struct process *proc) {
	space_load(proc->space);
}
