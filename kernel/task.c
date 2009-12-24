/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <mem.h>

pool_t *tpool;						/* Pool allocator for task structures */
task_t *task = (void*) TASK_TBL; 	/* Array of task structures (max 65536) */
pid_t curr_pid = 0;
task_t *curr_task = (void*) TASK_TBL;

task_t *task_get(pid_t pid) {
	if (pid >= MAX_TASKS) return NULL;
	if ((page_get((uint32_t) &task[pid]) & PF_PRES) == 0)
		return NULL;
	return (task[pid].magic == 0x4224) ? &task[pid] : NULL;
}

/* Used to initialize new tasks that are not marked yet */
void task_touch(pid_t pid) {
	if (pid >= MAX_TASKS) return;
	mem_alloc((uintptr_t) &task[pid], sizeof(task_t), 0x3);
	memclr((void*) &task[pid], sizeof(task_t)); 
	task[pid].magic = 0x4224;
}

/* Copy a task */
task_t *task_new(task_t *src) {
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
}

/* Delete a task, except for its address space */
uint32_t task_rem(task_t *t) {
	sched_rem(t->pid);
	t->magic = 0x0000;
	return pool_free(tpool, t->pid);
}

/* Switch to another task */
image_t *task_switch(task_t *t) {
	extern void list_sched(void);
	extern void fpu_save(uint32_t *fxdata);
	extern void fpu_load(uint32_t *fxdata);

	if (t->pid == curr_pid) return t->image;
	fpu_save(curr_task->image->fxdata);

	curr_task = t;
	curr_pid = t->pid;
	map_load(t->map);

	fpu_load(t->image->fxdata);

	return t->image;
}
