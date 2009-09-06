// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>
#include <init.h>

void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags);

struct idt_entry {
	uint16_t base_l;
	uint16_t seg;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_h;
} __attribute__((packed)) idt[96];

handler_t int_handlers[96];

extern void 
 int0(void),  int1(void),  int2(void),  int3(void),  int4(void),  int5(void),  int6(void),  
 int7(void),  int8(void),  int9(void), int10(void), int11(void), int12(void), int13(void), 
int14(void), int15(void), int16(void), int17(void), int18(void), int32(void), int33(void), 
int34(void), int35(void), int36(void), int37(void), int38(void), int39(void), int40(void), 
int41(void), int42(void), int43(void), int44(void), int45(void), int46(void), int47(void),
int64(void), int65(void), int66(void), int67(void), int68(void), int69(void), int70(void), 
int71(void), int72(void), int80(void), int81(void), int82(void), int83(void), int84(void);

typedef void (*int_handler_t) (void);
__attribute__ ((section(".idata")))
int_handler_t idt_raw[] = {

// Faults
int0, 	int1, 	int2, 	int3, 	int4, 	int5, 	int6, 	int7, 
int8, 	int9, 	int10, 	int11, 	int12, 	int13, 	int14, 	int15, 
int16,	int17, 	int18, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

// IRQs
int32,	int33, 	int34, 	int35, 	int36, 	int37, 	int38, 	int39, 
int40,	int41, 	int42, 	int43, 	int44, 	int45, 	int46, 	int47, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

// User system calls
int64, 	int65, 	int66, 	int67, 	int68, 	int69, 	int70, 	int71,
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

// Administrative System Calls
int80, 	int81, 	int82, 	int83,	int84,	NULL,	NULL,	NULL,
NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL

};

__attribute__ ((section(".itext")))
void init_idt() {
	uint8_t i;
	memclr(int_handlers, sizeof(handler_t) * 96);
	memclr(idt, sizeof(struct idt_entry) * 96);
	for (i = 0; i < 48; i++) if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
	for (i = 64;i < 96; i++) if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
	extern void idt_flush(void);
	idt_flush();
}

__attribute__ ((section(".ttext")))
void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags) {
	if (!base) return;
	idt[n].base_l = (uint16_t) (base & 0xFFFF);
	idt[n].base_h = (uint16_t) (base >> 16);
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

void register_int(uint8_t n, handler_t handler) {
	int_handlers[n] = handler;
}

void *int_handler(image_t *image) {
	task_t *t = task_get(curr_pid);

	if (image->num == 32 && image->cs & 0x3) t->image = image;
	tss_set_esp((uint32_t) image);

	if (image->num >= 32 && image->num <= 47) {
		if (image->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	if (int_handlers[image->num]) {
		if (image->num >= 0x50 && (t->flags & TF_SUPER) == 0) ret(image, EPERMIT);
		image = int_handlers[image->num](image);
	}

	return image;
}

struct tss {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[15];
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt;
	uint16_t trap, iomap_base;
} __attribute__ ((packed)) tss;

extern uint8_t gdt[48];

__attribute__ ((section(".ttext")))
void init_tss() {
	uint32_t base = (uint32_t) &tss;
	uint16_t limit = (uint16_t) (base + sizeof(struct tss));

	memclr(&tss, sizeof(struct tss));
	tss.cs = 0x0B;
	tss.ss0 = tss.es = tss.ds = tss.fs = tss.gs = 0x10;

	gdt[41] = (uint8_t) ((limit >> 8) & 0xFF);
	gdt[40] = (uint8_t) ((limit) & 0xFF);
	gdt[43] = (uint8_t) ((base >> 8) & 0xFF);
	gdt[42] = (uint8_t) (base & 0xFF);
	gdt[44] = (uint8_t) ((base >> 16) & 0xFF);
	gdt[47] = (uint8_t) ((base >> 24) & 0xFF);

	extern void tss_flush(void);
	tss_flush();
}

void tss_set_esp(uint32_t esp) {
	tss.esp0 = esp;
}
