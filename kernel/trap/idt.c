// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags);

struct idt_entry {
	uint16_t base_l;
	uint16_t seg;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_h;
} __attribute__((packed)) idt[96];

handler_t int_handlers[96];

extern void int0(),  int1(),  int2(),  int3(),  int4(),  int5(),  int6(),  int7(),  int8(),  int9(), int10(), int11(), int12(), int13(), int14(), int15(), int16(), int17(), int18();
extern void int32(), int33(), int34(), int35(), int36(), int37(), int38(), int39(), int40(), int41(), int42(), int43(), int44(), int45(), int46(), int47();
extern void int64(), int65(), int66(), int67(), int68(), int69(), int70(), int71();
extern void int80(), int81(), int82(), int83(), int84();

typedef void (*int_handler_t) (void);
__attribute__ ((section(".tdata")))
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

__attribute__ ((section(".ttext")))
void init_idt() {
	uint32_t i;
	memset((uint8_t*) &idt[0], 0, sizeof(struct idt_entry) * 256);
	for (i = 0; i <= 47; i++) if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
	for (i = 64;i <= 96; i++) if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
	extern void idt_flush();
	idt_flush();
}

__attribute__ ((section(".ttext")))
void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags) {
	if (!base) return;
	idt[n].base_l = (uint32_t) base & 0xFFFF;
	idt[n].base_h = (uint32_t) base >> 16;
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

void register_int(uint8_t n, handler_t handler) {
	int_handlers[n] = handler;
}

void *int_handler(image_t *state) {
	task_t *t = get_task(curr_pid);

	if (state->cs & 0x3) t->image = state;

	if (state->num >= 32 && state->num <= 47) {
		if (state->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	if (int_handlers[state->num]) {
		if (state->num >= 0x50 && (t->flags & TF_SUPER) == 0) {
			state->eax = EPERMIT;
			return state; // Filter unprivileged tasks from admin calls
		}
		state = int_handlers[state->num](state);
	}

	return state;
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
	uint16_t limit = base + sizeof(struct tss);

	memclr(&tss, sizeof(struct tss));
	tss.cs = 0x0B;
	tss.ss0 = tss.es = tss.ds = tss.fs = tss.gs = 0x10;

	gdt[41] = (limit >> 8) & 0xFF;
	gdt[40] = (limit) & 0xFF;
	gdt[43] = (base >> 8) & 0xFF;
	gdt[42] = base & 0xFF;
	gdt[44] = (base >> 16) & 0xFF;
	gdt[47] = (base >> 24) & 0xFF;

	extern void tss_flush();
	tss_flush();
}

void tss_set_esp(uint32_t esp) {
	tss.esp0 = esp;
}
