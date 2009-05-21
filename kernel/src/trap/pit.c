// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

// Handles IRQ 0, and advances a simple counter used as a clock
image_t *pit_handler(image_t *state) {
	static u32int tick = 0;
	tick++;

	return task_switch(next_task(0));
}

