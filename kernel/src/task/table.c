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

u16int new_task(u16int src_pid) {
	u32int new_pid = pool_alloc(tmap);
	if (new_pid == 0) new_pid = pool_alloc(tmap);

	task_t *new = get_task(new_pid);
	task_t *src = get_task(src_pid);

	new->user = src->user;
	map_clone(&new->map, &src->map, 0);
	new->image = src->image;
	new->flags = src->flags;
	new->parent = src_pid;
	new->magic = 0x4224;
	new->tss_esp = src->tss_esp;

	insert_sched(new_pid);
	return new_pid;
}

u32int rem_task(u16int pid) {
	task_t *t = get_task(pid);

	t->magic = 0x0000;
	return pool_free(tmap, pid);
}

image_t *task_switch(u16int pid) {
	curr_pid = pid;
	task_t *t = get_task(pid);
	map_load(&t->map);
	tss_set_esp(t->tss_esp);
	return t->image;
}

