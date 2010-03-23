/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <mem.h>

pid_t curr_pid = 0;
task_t *curr_task = (void*) TASK_TBL;

task_t *task_get(pid_t pid) {
	return process_get(pid);
}

/* Used to initialize new tasks that are not marked yet */
void task_touch(pid_t pid) {
	process_touch(pid);
}

task_t *task_new(task_t *src) {
	return process_clone(src);
}

/* Delete a task, except for its address space */
uint32_t task_rem(task_t *t) {
	process_free(t);

	return 0;
}

/* Switch to another task */
thread_t *task_switch(task_t *t, uint32_t thread) {
	extern void fpu_save(uint32_t *fxdata);
	extern void fpu_load(uint32_t *fxdata);

	/* Save FPU state */
	if (curr_task->flags & CTRL_FLOAT) {
		fpu_save(curr_task->image[curr_task->curr_thread].fxdata);
	}

	/* switch task */
	curr_task = t;
	curr_pid = t->pid;
	space_load(t->space);

	/* switch thread */
	curr_task->curr_thread = thread;

	/* Load FPU state */
	if (curr_task->flags & CTRL_FLOAT) {
		fpu_load(curr_task->image[curr_task->curr_thread].fxdata);
	}

	return curr_task->image;
}
