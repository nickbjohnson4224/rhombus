// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

void idt_set(u8int n, u32int base, u16int seg, u8int flags);

struct idt_entry {
	u16int base_l;
	u16int seg;
	u8int  reserved;
	u8int  flags;
	u16int base_h;
} __attribute__((packed)) idt[96];

handler_t int_handlers[96];

extern void int0(),  int1(),  int2(),  int3(),  int4(),  int5(),  int6(),  int7(),  int8(),  int9();
extern void int10(), int11(), int12(), int13(), int14(), int15(), int16(), int17(), int18();
extern void int32(), int33(), int34(), int35(), int36(), int37(), int38(), int39(), int40();
extern void int41(), int42(), int43(), int44(), int45(), int46(), int47();
extern void int64(), int65(), int66(), int67(), int68(), int69(), int70(), int71();
extern void int80(), int81();

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

// System calls
int64, 	int65, 	int66, 	int67, 	int68, 	int69, 	int70, 	int71,
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 

// Administrative System Calls
int80, 	int81, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,
NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL

};

__attribute__ ((section(".ttext")))
void init_idt() {
	u32int i;
	memset((u8int*) &idt[0], 0, sizeof(struct idt_entry) * 256);
	for (i = 0; i <= 47; i++) if (idt_raw[i]) idt_set(i, (u32int) idt_raw[i], 0x08, 0x8E);
	for (i = 64;i <= 96; i++) if (idt_raw[i]) idt_set(i, (u32int) idt_raw[i], 0x08, 0xEE);
	extern void idt_flush();
	idt_flush();
}

__attribute__ ((section(".ttext")))
void idt_set(u8int n, u32int base, u16int seg, u8int flags) {
	if (!base) return;
	idt[n].base_l = (u32int) base & 0xFFFF;
	idt[n].base_h = (u32int) base >> 16;
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

void register_int(u8int n, handler_t handler) {
	int_handlers[n] = handler;
}

void *int_handler(image_t *state) {
	task_t *t = get_task(curr_pid);

	if (state->cs & 0x3) t->image = state;

	if (state->num >= 32 && state->num <= 47) {
		if (state->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	if (int_handlers[state->num])
		state = int_handlers[state->num](state);

//	printk("returning from %d to ip = %x, frame@ip = %x\n", state->num, state->eip, ctbl[state->eip >> 12]);

	return state;
}
