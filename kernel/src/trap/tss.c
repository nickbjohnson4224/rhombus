// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>

extern u8int gdt[48];

struct tss {
	u32int prev_tss;
	u32int esp0;
	u32int ss0;
	u32int unused0[15];
	u32int es, cs, ss, ds, fs, gs;
	u32int ldt;
	u16int trap, iomap_base;
} __attribute__ ((packed)) tss;

__attribute__ ((section(".ttext")))
void init_tss() {
	u32int base = (u32int) &tss;
	u16int limit = sizeof(struct tss);

	memclr(&tss, sizeof(struct tss));
	tss.ss0 = 0x13;
	extern u32int stack;
	tss.esp0 = (u32int) &stack;
	tss.cs = 0x0B;
	tss.es = tss.ds = tss.fs = tss.gs = 0x13;

	gdt[40] = (limit >> 8) & 0xFF;
	gdt[41] = (limit) & 0xFF;
	gdt[42] = (base >> 8) & 0xFF;
	gdt[43] = base & 0xFF;
	gdt[44] = (base >> 16) & 0xFF;
	gdt[47] = (base >> 24) & 0xFF;

	extern void tss_flush();
	tss_flush();
}
