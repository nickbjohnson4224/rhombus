/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <process.h>
#include <thread.h>
#include <string.h>
#include <space.h>
#include <cpu.h>
#include <irq.h>

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

static pid_t pidlist[MAX_TASKS];
static int pidlist_setup = 0;
static int pidlist_top = 0;
static int pidlist_bot = 0;

struct process *process_alloc(void) {
	uintptr_t i;
	pid_t pid;

	if (pidlist_setup == 0) {
		for (i = 0; i < MAX_TASKS; i++) {
			pidlist[i] = i;
		}
		pidlist_setup = 1;
	}

	pid = pidlist[pidlist_top];
	pidlist_top = (pidlist_top + 1) % MAX_TASKS;

	process_table[pid] = heap_alloc(sizeof(struct process));
	process_table[pid]->pid = pid;
	return process_table[pid];
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
	uint32_t pid;

	/* allocate new process structure for child */
	child = process_alloc();
	pid   = child->pid;

	/* copy parent */
	memcpy(child, parent, sizeof(struct process));

	/* setup child */
	child->space  = space_clone();
	child->parent = parent;
	child->pid    = pid;
	child->rirq   = IRQ_NULL;

	memclr(child->thread, sizeof(struct thread*) * 256);

	new_thread = thread_alloc();

	/* copy parent thread */
	if (active) {
		heap_free(new_thread->fxdata, 512);
		memcpy(new_thread, active, sizeof(struct thread));
		new_thread->fxdata = heap_alloc(512);

		/* copy parent FPU/SSE state */
		if (active->fxdata) {
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
	process_table[proc->pid] = NULL;

	pidlist[pidlist_bot] = proc->pid;
	pidlist_bot = (pidlist_bot + 1) % MAX_TASKS;

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

	if (proc->rirq != IRQ_NULL) {
		irq_set_redirect(0, proc->rirq);
	}

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
	cpu_set_cr3(proc->space);
}
