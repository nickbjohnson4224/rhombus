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

	/* switch task */
	curr_task = proc;
	curr_pid = proc->pid;
	space_load(proc->space);

	return proc->image;
}
