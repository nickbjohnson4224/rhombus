// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;

	task_t *t;
	t = get_task(curr_pid);
	t->image = state;

	u16int pid = next_task(0);
	printk("%d\t", pid);

	t = get_task(pid);
	printk("%x\n", t->map);
	return task_switch(pid);
}
