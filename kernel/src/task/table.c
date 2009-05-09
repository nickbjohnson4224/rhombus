// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

pool_t *tmap;		// Pool allocator for task structures
task_t *task[128]; 	// 2D array (128*128) of task structures
u16int curr_pid;

task_t *get_task(u16int pid) {
	if (!task[pid >> 7]) {
		task[pid >> 7] = kmalloc(0x1000);
		memclr(task[pid >> 7], 0x1000);
	}
	return &task[pid >> 7][pid & 0x7F];
}

u16int new_task(u16int src_pid) {
	u32int new_pid = pool_alloc(tmap);

	task_t *new = get_task(new_pid);
	task_t *src = get_task(src_pid);

	new->user = src->user;
	map_clone(&new->map, &src->map, 0);
	new->image = src->image;
	new->flags = src->flags;
	new->sigmask[0] = src->sigmask[0];
	new->sigmask[1] = src->sigmask[1];
	new->parent = src_pid;

	insert_sched(new_pid);
	return new_pid;
}

u32int rem_task(u16int pid) {
	return pool_free(tmap, pid);
}

image_t *task_switch(u16int pid) {
	curr_pid = pid;
	task_t *t = get_task(pid);
	map_load(&t->map);
	return t->image;
}
