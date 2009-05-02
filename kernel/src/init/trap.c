// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>

// Handles IRQ 0, and advances a simple counter used as a clock
void *pit_handler(image_t *state) {
	static u32int tick;
	tick++;
	return state;
}

__attribute__ ((section(".ttext"))) 
void init_pit() {
	colork(0x9);
	printk("Starting system timer (256 Hz)");

	register_int(IRQ(0), pit_handler);

	// Set the PIT to 256Hz
	u16int divisor = 1193180 / 256;
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xFF);
	outb(0x40, divisor >> 8);

	cursek(36, -1);
	printk("done\n");
}

__attribute__ ((section(".ttext"))) 
void init_int() {

	colork(0x9);
	printk("Creating interrupts");
	init_idt();
	cursek(36, -1);
	printk("done\n");

	printk("Re-mapping IRQs");

	// Complex remapping code - even I don't understand it well
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	cursek(36, -1);
	printk("done\n");

	printk("Setting up fault handlers");

	// Clear the interrupt handler table
	extern handler_t int_handlers[256];
	memclr(int_handlers, sizeof(handler_t) * 256);

	cursek(36, -1);
	printk("done\n");

	colork(0xF);
}
