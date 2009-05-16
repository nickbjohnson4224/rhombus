// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick = 0;
	tick++;

	task_t *t = get_task(curr_pid);
	if (state->eip < 0xF8000000) t->image = state;
	state = task_switch(next_task(0));

	printk("%x\t%x %x %x (%x %x %x %x)\n", curr_pid, state, state->useresp, state->eip,
		state->eax, state->ebx, state->ecx, state->edx);
	return state;
}

