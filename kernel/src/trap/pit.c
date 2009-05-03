// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;
	u16int pid = next_task();
	printk("%d\t", pid);
	task_t *t = get_task(pid);
	printk("%x\n", t->map);
	return task_switch(pid);
}
