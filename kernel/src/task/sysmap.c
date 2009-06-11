// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>
#include <init.h>

u16int irq_holder[15];

void init_sysmap() {
	printk("System map");

	memclr(irq_holder, sizeof(u16int) * 15);

	cursek(36, -1);
	printk("done\n");
}
