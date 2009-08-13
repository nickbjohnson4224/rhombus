// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <init.h>

__attribute__ ((section(".ttext"))) 
void init_task() {
	printk("  Kernel: tasking system");

		// Set up task table and allocator
		pool_new(MAX_TASKS, tpool);

		// Bootstrap task 0, because there is nothing to fork from
		task_t *idle = get_task(0);
		memclr(idle, sizeof(task_t));
		idle->pid = pool_alloc(tpool);
		idle->magic = 0x4224;
		idle->map = map_clone();
		idle->flags = TF_READY | TF_SUPER;

		// Make sure the scheduler works
		queue.next = 0;
		queue.last = 0;

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
