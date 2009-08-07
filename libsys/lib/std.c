// Copyright 2009 Nick Johnson

#include <lib.h>

void outb(u16int port, u8int val) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

u8int inb(u16int port) {
	u8int ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void sleep(u32int cycles) {
	u32int i;
	for (i = 0; i < cycles; i++);
}
