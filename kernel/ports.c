/* Copyright 2009 Nick Johnson */

#include <lib.h>

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void sleep(uint32_t cycles) {
	volatile uint32_t i;
	for (i = 0; i < cycles; i++);
}
