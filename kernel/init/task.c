// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <init.h>

__attribute__ ((section(".itext"))) 
void init_task() {
	printk("  Kernel: tasking system");

		// Set up task table and allocator
		tpool = pool_new(MAX_TASKS);

		// Bootstrap task 0, because there is nothing to fork from
		task_touch(0);
		task_t *idle = task_get(0);
		idle->pid = (pid_t) pool_alloc(tpool);
		idle->map = map_clone();
		idle->flags = TF_READY | TF_SUPER;
		tss_set_esp(SSTACK_INI);

		// Make sure the scheduler works
		queue.next = 0;
		queue.last = 0;

		// Fork task 1
		task_switch(task_new(task_get(0)));

		// Mark task 0 as gone
		idle->magic = 0;

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading init");

		// Load init from the initrd
		init_user_init();

	cursek(74, -1);
	printk("[done]");
}
