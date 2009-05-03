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

u32int new_task(task_t *src) {
	u32int new_pid = pool_alloc(tmap);
	task_t *task_ptr = get_task(new_pid);
	task_ptr->user = src->user;
	map_clone(&task_ptr->map, &src->map, 0);
	task_ptr->image = src->image;
	task_ptr->flags = src->flags;
	task_ptr->sigmask = src->sigmask;
	insert_sched(new_pid);
	return new_pid;
}

u32int rem_task(u16int pid) {
	return pool_free(tmap, pid);
}

image_t *task_switch(u16int pid) {
	if (!task[pid >> 7]) panic("no such task");
	curr_pid = pid;
	task_t *t = get_task(pid);
	map_load(&t->map);
	return t->image;
}

