// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>

__attribute__ ((section(".ttext"))) 
void init_pit() {
	colork(0x9);
	printk("System timer (256 Hz)");

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
	printk("Interrupts");
	init_idt();

	// Complex remapping code - even I don't understand it well
	// Essentially, it moves all IRQs to interrupts 32-47.
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);

	cursek(36, -1);
	printk("done\n");

	printk("Fault handlers");

	// Clear the interrupt handler table
	extern handler_t int_handlers[256];
	memclr(int_handlers, sizeof(handler_t) * 256);

	// Initialize the TSS
	init_tss();

	cursek(36, -1);
	printk("done\n");

	colork(0xF);
}
