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
	if (tick % 16 == 0) new_task(t);

	u16int pid = next_task(0);
	if (pid) printk("%d\n", pid);
	
	state = task_switch(pid);
	return state;
}
