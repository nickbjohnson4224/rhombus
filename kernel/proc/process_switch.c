/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <int.h>
#include <task.h>

/****************************************************************************
 * process_switch
 *
 * Switches to the target process and its specified thread.
 */

struct thread *process_switch(struct process *proc, uint32_t thread) {
	extern void fpu_save(uint32_t *fxdata);
	extern void fpu_load(uint32_t *fxdata);

	/* Save FPU state */
	if (curr_task->flags & CTRL_FLOAT) {
		fpu_save(curr_task->image[curr_task->curr_thread].fxdata);
	}

	/* switch task */
	curr_task = proc;
	curr_pid = proc->pid;
	space_load(proc->space);

	/* switch thread */
	curr_task->curr_thread = thread;

	/* Load FPU state */
	if (curr_task->flags & CTRL_FLOAT) {
		fpu_load(curr_task->image[curr_task->curr_thread].fxdata);
	}

	return &curr_task->image[curr_task->curr_thread];
}
