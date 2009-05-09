// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;

	task_t *t = get_task(curr_pid);
	if (state < 0xF8000000) // Ignore stuff on the init stack
		t->image = state;
	
	state = task_switch(next_task(0));
	printk("%x %x %x %x\n", state, state->eip, state->cs, state->eax);
	return state;
}

