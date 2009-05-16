// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>

__attribute__ ((section(".ttext"))) 
void init_task() {
	colork(0xA);
	printk("Tasking system");

		// Set up task table and allocator
		memclr(task, sizeof(task_t*) * 64);
		tmap = pool_new(128*128);

		// Bootstrap task 0, because there is nothing to fork from
		task_t *idle = get_task(0);
		idle->user.id = 0;
		idle->user.ring = 0;
		map_clone(&idle->map, &kmap, MF_CLEAR_USER);
		idle->flags = TF_BLOCK;
		idle->sigmask[0] = 0x00000000;
		idle->sigmask[1] = 0x00000000;
		curr_pid = 0;

		// Make sure the scheduler works
		init_sched();
	
	cursek(36, -1);
	printk("done\n");
	printk("System library");

		// Load the system library from the initrd
		init_libsys();

	cursek(36, -1);
	printk("done\n");
	colork(0xF);
}
