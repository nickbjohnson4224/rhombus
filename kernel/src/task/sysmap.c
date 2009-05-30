// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>
#include <init.h>

struct sysmap *sysmap;

u16int irq_holder[15] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static image_t *irq_redirect(image_t *image) {
//	printk("redirecting IRQ %d to task %d\n", DEIRQ(image->num), irq_holder[DEIRQ(image->num)]);
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0);
}

void init_sysmap() {
	printk("System map");

	task_t *t = get_task(curr_pid);
	u32int i;

	// Allocate space for map and clear
	for(i = 0xF5FF0000; i < 0xF5FF0000 + sizeof(struct sysmap); i += 0x1000)
		p_alloc(&t->map, i, (PF_USER | PF_RW));
	sysmap = (void*) 0xF5FF0000;
	memclr(sysmap, sizeof(struct sysmap));

	// Register handlers for generic IRQs
	register_int(IRQ(1), irq_redirect);

	cursek(36, -1);
	printk("done\n");
}
