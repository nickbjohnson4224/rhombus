/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <util.h>
#include <time.h>
#include <space.h>
#include <init.h>

void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags);

/* Interrupt Descriptor Table */
struct idt_entry {
	uint16_t base_l;
	uint16_t seg;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_h;
} __attribute__((packed)) idt[96];

/* Redirection table for handling interrupts */
handler_t int_handlers[96];

/* Assembly interrupt handler stubs to be registered in the IDT */
extern void
 int0(void),  int1(void),  int2(void),  int3(void),  int4(void),  int5(void),  
 int6(void),  int7(void),  int8(void),  int9(void),  int10(void), int11(void), 
 int12(void), int13(void), int14(void), int15(void), int16(void), int17(void), 
 int18(void), int19(void), int32(void), int33(void), int34(void), int35(void), 
 int36(void), int37(void), int38(void), int39(void), int40(void), int41(void), 
 int42(void), int43(void), int44(void), int45(void), int46(void), int47(void), 
 int64(void), int65(void), int66(void), int67(void), int72(void), int73(void), 
 int74(void), int75(void), int80(void), int81(void);

/* Handlers to be put into the IDT, in order */
typedef void (*int_handler_t) (void);
int_handler_t idt_raw[] = {

/* Faults */
int0, 	int1, 	int2, 	int3, 	int4, 	int5, 	int6, 	int7, 
int8, 	int9, 	int10, 	int11, 	int12, 	int13, 	int14, 	int15, 
int16,	int17, 	int18, 	int19, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

/* IRQs */
int32,	int33, 	int34, 	int35, 	int36, 	int37, 	int38, 	int39, 
int40,	int41, 	int42, 	int43, 	int44, 	int45, 	int46, 	int47, 

NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

/* ABI 4 system calls */
int64, 	int65, 	int66, 	int67, 	NULL, 	NULL, 	NULL, 	NULL, 
int72, 	int73, 	int74, 	int75, 	NULL, 	NULL, 	NULL, 	NULL, 
int80, 	int81, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,

};

/* Create the IDT from idt_raw[] */
void init_idt() {
	extern void idt_flush(void);
	extern void halt(void);
	size_t i;

	/* Write privileged interrupt handlers (faults, IRQs) */
	for (i = 0; i < 48; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
	}

	/* Write usermode interrupt handlers (syscalls) */
	for (i = 64; i < 96; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
	}

	/* Write the IDT */
	idt_flush();
}

/* Set an IDT entry to a value */
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
thread_t *int_handler(thread_t *image) {
	thread_t *new_image;

	/* Reset PIC if it was an IRQ */
	if (image->num >= 32 && image->num <= 47) {
		if (image->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	/* Call registered C interrupt handler from int_handlers[] table */
	if (int_handlers[image->num]) {
		new_image = int_handlers[image->num](image);
	}

	if (new_image != image) {

		/* Set TSS to generate images in the proper position */
		tss_set_esp((uintptr_t) &new_image->tss_start);

		return thread_switch(image, new_image);
	}

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

/* Toggle IRQ mask for PIC */
void pic_mask(uint16_t mask) {
	static uint16_t currmask = 0xFFFF;
	uint8_t smask, mmask;

	/* Toggle bits */
	currmask ^= mask;
	smask = (currmask >> 0) & 0xFF;
	mmask = (currmask >> 8) & 0xFF;

	/* (re)initialize 8259 PIC */
	outb(0x20, 0x11); /* Initialize master */
	outb(0xA0, 0x11); /* Initialize slave */
	outb(0x21, 0x20); /* Master mapped to 0x20 - 0x27 */
	outb(0xA1, 0x28); /* Slave mapped to 0x28 - 0x2E */
	outb(0x21, 0x04); /* Master thingy */
	outb(0xA1, 0x02); /* Slave thingy */
	outb(0x21, 0x01); /* 8086 (standard) mode */
	outb(0xA1, 0x01); /* 8086 (standard) mode */
	outb(0x21, smask); /* Master IRQ mask */
	outb(0xA1, mmask); /* Slave IRQ mask */
}
