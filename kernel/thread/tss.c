/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

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
