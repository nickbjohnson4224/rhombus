/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <init.h>

__attribute__ ((section(".ttext"))) 
void init_fault() {

	register_int(0,  fault_float);
	register_int(1,  fault_generic);
	register_int(2,  fault_generic);
	register_int(3,  fault_generic);
	register_int(4,  fault_generic);
	register_int(5,  fault_generic);
	register_int(6,  fault_generic);
	register_int(7,  fault_float);
	register_int(8,  fault_double);
	register_int(9,  fault_float);
	register_int(10, fault_generic);
	register_int(11, fault_generic);
	register_int(12, fault_generic);
	register_int(13, fault_generic);
	register_int(14, fault_page);
	register_int(15, fault_generic);
	register_int(16, fault_float);
	register_int(17, fault_generic);
	register_int(18, fault_generic);

}

__attribute__ ((section(".ttext"))) 
void init_pit() {
	uint16_t divisor;
	printk("  Kernel: system timer @ 256Hz");

		/* Register IRQ handler (PIT == IRQ 0) */
		register_int(IRQ(0), pit_handler);

		/* Set the PIT frequency to 256Hz */
		divisor = 1193180 / 256;
		outb(0x43, 0x36);
		outb(0x40, (uint8_t) (divisor & 0xFF));
		outb(0x40, (uint8_t) (divisor >> 8));

	cursek(74, -1);
	printk("[done]");
}

uint16_t irq_holder[15];

__attribute__ ((section(".ttext"))) 
void init_int() {
	extern handler_t int_handlers[96];

	printk("  Kernel: interrupts");

		/* Set up interupt descriptor table */
		init_idt();

		/* Complex remapping code - even I don't understand it well */
		/* Essentially, it moves all IRQs to interrupts 32-47. */
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

		/* Clear IRQ redirection table */
		memclr(irq_holder, sizeof(pid_t) * 15);

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: system calls");

		/* Clear the interrupt handler table */
		memclr(int_handlers, sizeof(handler_t) * 96);

		/* Initialize the TSS */
		init_tss();

		/* System calls */
		register_int(0x40, fork_call);	/* Fork current task */
		register_int(0x41, exit_call);	/* End current task */
		register_int(0x42, gpid_call);	/* Get current PID */
		register_int(0x43, tblk_call);	/* Block scheduling */
		register_int(0x44, mmap_call);	/* Allocate memory to a page */
		register_int(0x45, umap_call);	/* Free memory from a page */
		register_int(0x46, ssnd_call);	/* Send signal to task */
		register_int(0x47, sret_call);	/* Return from signal handler */
		register_int(0x48, sblk_call);	/* Block signals */
		register_int(0x49, sreg_call);	/* Register a signal handler */

		/* Administrative system calls */
		register_int(0x50, ireg_call);	/* Register IRQ */
		register_int(0x51, irel_call);	/* Deregister IRQ */
		register_int(0x52, push_call);	/* Copy memory to another address space */
		register_int(0x53, pull_call);	/* Copy memory from another address space */
		register_int(0x54, eout_call);	/* Emergency output */

		/* Register fault handlers */
		init_fault();

	cursek(74, -1);
	printk("[done]");
}
