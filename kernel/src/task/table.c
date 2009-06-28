// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

pool_t tpool[64];						// Pool allocator for task structures
task_t *task = (void*) 0xFF400000; 		// Array of task structures (max 8192)
u16int curr_pid = 0;

task_t *get_task(u16int pid) {
	if ((page_get((u32int) &task[pid]) & 0x1) == 0) {
		p_alloc((u32int) &task[pid], PF_PRES | PF_RW);
		pgclr(&task[pid]);
	}
	return &task[pid];
}

task_t *new_task(task_t *src) {
	u32int new_pid = pool_alloc(tpool);
	if (new_pid == 0) new_pid = pool_alloc(tpool);

	task_t *new = get_task(new_pid);

	new->user = src->user;
	new->map = map_clone();
	new->image = src->image;
	new->flags = src->flags;
	new->dlist[0] = src->pid;
	new->pid = new_pid;
	new->magic = 0x4224;
	new->tss_esp = src->tss_esp;

	insert_sched(new_pid);
	return new;
}

u32int rem_task(task_t *t) {
	t->magic = 0x0000;
	return pool_free(tpool, t->pid);
}

image_t *task_switch(task_t *t) {
	curr_pid = t->pid;
	map_load(t->map);
	tss_set_esp(t->tss_esp);
	return t->image;
}

