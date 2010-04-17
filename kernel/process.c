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

process_t *process_clone(process_t *parent, thread_t *active_thread) {
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

	memclr(child->thread_stack_bmap, sizeof(uint32_t) * 4);
	memclr(child->thread, sizeof(struct thread *) * 128);

	child->thread[0] = thread_alloc();

	/* copy parent thread */
	if (active_thread) {
		memcpy(child->thread[0], active_thread, sizeof(thread_t));

		/* copy parent FPU/SSE state */
		if (active_thread->fxdata) {
			child->thread[0]->fxdata = heap_alloc(512);
			memcpy(child->thread[0]->fxdata, active_thread->fxdata, 512);
		}
	}

	/* setup child thread */
	child->thread[0]->stack = thread_stack_alloc(child->thread[0], child);
	child->thread[0]->proc  = child;

	/* add child's thread to the scheduler */
	schedule_insert(child->thread[0]);

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
	idle->flags = CTRL_READY | CTRL_SUPER;

	/* fork process 1 and switch */
	process_switch(process_clone(idle, NULL));
}

/****************************************************************************
 * process_kill
 *
 * Frees a process entirely.
 */

void process_kill(process_t *proc) {
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
