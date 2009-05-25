// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

pool_t *tmap;		// Pool allocator for task structures
task_t *task[128]; 	// 2D array (128*128) of task structures
u16int curr_pid = 0;

task_t *get_task(u16int pid) {
	if (!task[pid >> 7]) {
		task[pid >> 7] = kmalloc(0x1000);
		memclr(task[pid >> 7], 0x1000);
	}
	return &task[pid >> 7][pid & 0x7F];
}

task_t *new_task(task_t *src) {
	u32int new_pid = pool_alloc(tmap);
	if (new_pid == 0) new_pid = pool_alloc(tmap);

	task_t *new = get_task(new_pid);

	new->user = src->user;
	map_clone(&new->map, &src->map, 0);
	new->image = src->image;
	new->flags = src->flags;
	new->parent = src->pid;
	new->pid = new_pid;
	new->magic = 0x4224;
	new->tss_esp = src->tss_esp;

	insert_sched(new_pid);
	return new;
}

u32int rem_task(task_t *t) {
	t->magic = 0x0000;
	return pool_free(tmap, t->pid);
}

image_t *task_switch(task_t *t) {
	curr_pid = t->pid;
	map_load(&t->map);
	tss_set_esp(t->tss_esp);
	return t->image;
}

