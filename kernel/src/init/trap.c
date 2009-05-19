// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

image_t *fork_call(image_t *image) {
	printk("fork()\n");
	image->eax = 0;
	u16int parent = curr_pid;
	image = task_switch(new_task(curr_pid));
	image->eax = parent;
	return image;
}

image_t *sint_call(image_t *image) {
	printk("sint() to %d\n", image->eax);
	sleep(1000000);
	return signal(image->eax, image->esi, image->ebx, image->ecx, image->edx, image->edi);
}

image_t *sret_call(image_t *image) {
	printk("sret()\n");
	return sret();
}

__attribute__ ((section(".ttext"))) 
void init_pit() {
	u16int divisor;
	colork(0x9);
	printk("System timer (256 Hz)");

		// Register IRQ handler (PIT == IRQ 0)
		register_int(IRQ(0), pit_handler);

		// Set the PIT frequency to 256Hz
		divisor = 1193180 / 256;
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

		// Set up interupt descriptor table
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
	printk("System calls");

		// Clear the interrupt handler table
		extern handler_t int_handlers[256];
		memclr(int_handlers, sizeof(handler_t) * 256);

		// Initialize the TSS
		init_tss();

		// Register system calls
		register_int(0x40, fork_call);
		register_int(0x42, sint_call);
		register_int(0x43, sret_call);

	cursek(36, -1);
	printk("done\n");
	colork(0xF);
}
