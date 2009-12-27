/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <mem.h>
#include <init.h>

void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags);

/* Interrupt Descriptor Table */
struct idt_entry {
	uint16_t base_l;
	uint16_t seg;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_h;
} __attribute__((packed)) idt[128];

/* Redirection table for handling interrupts */
handler_t int_handlers[128];

/* Assembly interrupt handler stubs to be registered in the IDT */
extern void 
 int0(void),  int1(void),  int2(void),  int3(void),  int4(void),  int5(void),  int6(void),  
 int7(void),  int8(void),  int9(void), int10(void), int11(void), int12(void), int13(void), 
int14(void), int15(void), int16(void), int17(void), int18(void), int32(void), int33(void), 
int34(void), int35(void), int36(void), int37(void), int38(void), int39(void), int40(void), 
int41(void), int42(void), int43(void), int44(void), int45(void), int46(void), int47(void),
int64(void), int65(void), int66(void), int67(void), int68(void), int69(void), int70(void), 
int71(void), int72(void), int73(void), int80(void), int81(void), int82(void), int83(void), 
int84(void), int96(void), int97(void), int98(void), int99(void), int100(void), int101(void),
int102(void), int103(void);

/* Handlers to be put into the IDT, in order */
typedef void (*int_handler_t) (void);
__attribute__ ((section(".idata")))
int_handler_t idt_raw[] = {

/* Faults */
int0, 	int1, 	int2, 	int3, 	int4, 	int5, 	int6, 	int7, 
int8, 	int9, 	int10, 	int11, 	int12, 	int13, 	int14, 	int15, 
int16,	int17, 	int18, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

/* IRQs */
int32,	int33, 	int34, 	int35, 	int36, 	int37, 	int38, 	int39, 
int40,	int41, 	int42, 	int43, 	int44, 	int45, 	int46, 	int47, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

/* User system calls */
int64, 	int65, 	int66, 	int67, 	int68, 	int69, 	int70, 	int71,
int72, 	int73, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL,

/* Administrative System Calls */
int80, 	int81, 	int82, 	int83,	int84,	NULL,	NULL,	NULL,
NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,

/* ABI 2 system calls */
int96,	int97,	int98,	int99,	int100,	int101,	int102,	int103,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL

};

/* Create the IDT from idt_raw[] */
__attribute__ ((section(".itext")))
void init_idt() {
	extern void idt_flush(void);
	uint8_t i;

	memclr(int_handlers, sizeof(handler_t) * 128);
	memclr(idt, sizeof(struct idt_entry) * 128);

	/* Write privileged interrupt handlers (faults, IRQs) */
	for (i = 0; i < 48; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
	}
	
	/* Write usermode interrupt handlers (syscalls) */
	for (i = 64;i< 128; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
	}

	/* Write the IDT */
	idt_flush();
}

/* Set an IDT entry to a value */
__attribute__ ((section(".itext")))
void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags) {
	if (!base) return; /* Ignore null handlers */
	idt[n].base_l = (uint16_t) (base & 0xFFFF);
	idt[n].base_h = (uint16_t) (base >> 16);
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

/* Register an interrupt handler in C to be redirected to on an interrupt */
void register_int(uint8_t n, handler_t handler) {
	int_handlers[n] = handler;
}

/* C interrupt handler - called by assembly state-saving routine */
void *int_handler(image_t *image) {
	extern void page_flush_full();
	task_t *t = curr_task;

	/* If userspace was interrupted, make sure its state is saved */
	if (image->cs & 0x3) t->image = image;

	/* Reset PIC if it was an IRQ */
	if (image->num >= 32 && image->num <= 47) {
		if (image->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	/* Call registered C interrupt handler from int_handlers[] table */
	if (int_handlers[image->num]) {
		image = int_handlers[image->num](image);
	}

	/* Check image checksum */
	if (!page_get((uintptr_t) &image->mg) || image->mg != 0x42242442) {
		printk("%x: \n", image);
		printk("%x\n", page_get((uintptr_t) (image + 0x1000)));
		panic("invalid image");
	}

	/* Set TSS to generate images in the proper position */
	tss_set_esp((uintptr_t) image + sizeof(image_t));

	return image;
}

/* Task State Segment */
struct tss {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[15];
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt;
	uint16_t trap, iomap_base;
} __attribute__ ((packed)) tss;

/* Global Descriptor Table - defined in boot.s */
extern uint8_t gdt[48];

/* Initialize the TSS */
__attribute__ ((section(".ttext")))
void init_tss() {
	extern void tss_flush(void);
	uint32_t base = (uint32_t) &tss;
	uint16_t limit = (uint16_t) (base + sizeof(struct tss));

	memclr(&tss, sizeof(struct tss));
	tss.cs = 0x0B;
	tss.ss0 = tss.es = tss.ds = tss.fs = tss.gs = 0x10;

	/* Change the 6th GDT entry to be the TSS */
	gdt[40] = (uint8_t) ((limit) & 0xFF);
	gdt[41] = (uint8_t) ((limit >> 8) & 0xFF);
	gdt[42] = (uint8_t) (base & 0xFF);
	gdt[43] = (uint8_t) ((base >> 8) & 0xFF);
	gdt[44] = (uint8_t) ((base >> 16) & 0xFF);
	gdt[47] = (uint8_t) ((base >> 24) & 0xFF);

	tss_flush();
}

/* Set the ESP0 value of the TSS - used to define the top of the TIS */
void tss_set_esp(uint32_t esp) {
	tss.esp0 = esp;
}
