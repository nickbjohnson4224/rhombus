/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <mem.h>

pool_t *tpool;
task_t *task = (void*) TASK_TBL; 	/* Array of task structures */
pid_t curr_pid = 0;
task_t *curr_task = (void*) TASK_TBL;

process_t *process_get(pid_t pid) {
	uintptr_t addr;

	if (pid >= MAX_PID) {
		return NULL;
	}

	addr = PROCESS_TABLE + (PAGESZ * pid);

	if (page_get(addr) & PF_PRES) {
		return (process_t*) addr;
	}

	return NULL;
}

void process_touch(pid_t pid) {
	uintptr_t addr;

	if (pid >= MAX_PID) {
		return;
	}

	addr = PROCESS_TABLE + (PAGESZ * pid);

	if (page_get(addr) & PF_PRES) {
		return;
	}
	page_set(addr, page_fmt(frame_new(), PF_PRES | PF_RW));
}

void process_free(process_t *proc) {
	uintptr_t addr;

	addr = (uintptr_t) proc;

	if (addr < PROCESS_TABLE || addr >= PROCESS_TABLE + (PAGESZ * MAX_PID)) {
		return;
	}

	frame_free(page_get(addr));
	page_set(addr, 0);
}

process_t *process_alloc(void) {
	uintptr_t addr;
	process_t *proc;
	pid_t pid;

	for (pid = 0; pid < MAX_PID; pid++) {
		addr = PROCESS_TABLE + (PAGESZ * pid);

		if ((page_get(addr) & PF_PRES) == 0) {
			process_touch(pid);

			proc = (process_t*) addr;
			proc->pid = pid;

			return proc;
		}
	}

	return NULL;
}

process_t *process_clone(process_t *parent) {
	process_t *child;
	pid_t pid;

	child = process_alloc();
	pid   = child->pid;

	memcpy(child, parent, sizeof(process_t));

	child->map    = map_clone();
	child->parent = parent->pid;
	child->pid    = pid;

	sched_ins(pid);

	return child;
}

void process_kill(process_t *proc) {
	sched_rem(proc->pid);

	process_free(proc);
}

task_t *task_get(pid_t pid) {
/*	if (pid >= MAX_TASKS) return NULL;
	if ((page_get((uint32_t) &task[pid]) & PF_PRES) == 0)
		return NULL;
	return (task[pid].magic == 0x4224) ? &task[pid] : NULL; */
	return process_get(pid);
}

/* Used to initialize new tasks that are not marked yet */
void task_touch(pid_t pid) {
/*	if (pid >= MAX_TASKS) return;
	mem_alloc((uintptr_t) &task[pid], sizeof(task_t), 0x3);
	memclr((void*) &task[pid], sizeof(task_t)); 
	task[pid].magic = 0x4224; */
	process_touch(pid);
}

task_t *task_new(task_t *src) {
	return process_clone(src);
}

/* Copy a task */
/*task_t *task_new(task_t *src) {
	pid_t new_pid;
	task_t *new;

	if (!src) panic ("bad parent");
	new_pid = pool_alloc(tpool);
	task_touch(new_pid);
	new = task_get(new_pid);
	if (!new) panic ("bad pid");

	memcpy(new, src, sizeof(task_t));
	new->map = map_clone();
	new->parent = src->pid;
	new->pid = new_pid;

	sched_ins(new_pid);
	return new;
} */

/* Delete a task, except for its address space */
uint32_t task_rem(task_t *t) {
	uint32_t err;

	sched_rem(t->pid);
	err = pool_free(tpool, t->pid);

	memclr(t, sizeof(task_t));
	return err;
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
	map_load(t->map);

	/* switch thread */
	curr_task->curr_thread = thread;

	/* Load FPU state */
	if (curr_task->flags & CTRL_FLOAT) {
		fpu_load(curr_task->image[curr_task->curr_thread].fxdata);
	}

	return curr_task->image;
}
