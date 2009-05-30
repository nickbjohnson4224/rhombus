// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>
#include <init.h>

//struct sysmap *sysmap;

void init_sysmap() {
	printk("System map");

	task_t *t = get_task(curr_pid);
	u32int i;

	// Allocate space for map and clear
//	for(i = 0xF5FF0000; i < 0xF5FF0000 + sizeof(struct sysmap); i += 0x1000)
//		p_alloc(&t->map, i, (PF_USER | PF_RW));
//	sysmap = (void*) 0xF5FF0000;
//	memclr(sysmap, sizeof(struct sysmap));

	memclr(irq_holder, sizeof(u16int) * 15);

	cursek(36, -1);
	printk("done\n");
}
