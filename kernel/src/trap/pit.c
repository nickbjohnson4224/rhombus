// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;
	u32int pid;
	if (tick & 1) pid = new_task(get_task(0));
	else pid = 0;
	printk("%d\t", pid);
	task_t *t = get_task(pid);
	printk("%x\n", t->map);
	return task_switch(pid);
}
