// Copyright 2009 Nick Johnson

#include <lib.h>

void panic(char *message) {
	printk("\nKernel panic: %s\n", message);
	asm volatile ("cli");
	asm volatile ("hlt");
}
