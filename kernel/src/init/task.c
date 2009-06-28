// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <init.h>

__attribute__ ((section(".ttext")))
static void init_sched() {
	queue.next = 0;
	queue.last = 0;
}

__attribute__ ((section(".ttext"))) 
void init_task() {
	printk("  Kernel: tasking system");

		// Set up task table and allocator
		pool_new(65536, tpool);

		// Bootstrap task 0, because there is nothing to fork from
		task_t *idle = get_task(0);
		idle->user.id = 0;
		idle->user.ring = 0;
		idle->dlist[0] = 0;
		idle->pid = 0;
		idle->magic = 0x4224;
		idle->map = map_clone();
		idle->flags = TF_READY;
		curr_pid = 0;

		// Make sure the scheduler works
		init_sched();

		// Set the signal table pointer
		signal_table = (void*) 0xF3FFF000;

		// Fork task 1
		task_switch(new_task(get_task(curr_pid)));

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading libsys");

		// Load the system library from the initrd
		init_libsys();

	cursek(74, -1);
	printk("[done]");
}
