/* Copyright 2009 Nick Johnson */

#include <lib.h>

void panic(const char *message) {
	extern void halt(void);

	colork(0x7);
	printk("Kernel panic: %s\n", message);
	halt();
}
