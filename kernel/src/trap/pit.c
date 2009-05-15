// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;

	if (tick % 40 == 0) {
		printk("%x %x %x %x (%x %x %x %x)\n", state, state->magic, state->eip, state->cs, 
			state->eax, state->ebx, state->ecx, state->edx);
		task_t *t = get_task(0);
		if (state->eip < 0xF8000000)
			t->image = state;
	
		state = task_switch(0);
		printk("%x %x %x %x (%x %x %x %x)\n", state, state->magic, state->eip, state->cs, 
			state->eax, state->ebx, state->ecx, state->edx);
	}
	return state;
}

