/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <kernel/thread.h>

static void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags);

/* Interrupt Descriptor Table */
struct idt_entry {
	uint16_t base_l;
	uint16_t seg;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_h;
} __attribute__((packed)) idt[256];

/* Assembly interrupt handler stubs to be registered in the IDT */
extern void 
int0(void),   int1(void),   int2(void),   int3(void),   int4(void),
int5(void),   int6(void),   int7(void),   int8(void),   int9(void), 
int10(void),  int11(void),  int12(void),  int13(void),  int14(void), 
int15(void),  int16(void),  int17(void),  int18(void),  int32(void), 
int33(void),  int34(void),  int35(void),  int36(void),  int37(void), 
int38(void),  int39(void),  int40(void),  int41(void),  int42(void), 
int43(void),  int44(void),  int45(void),  int46(void),  int47(void),
int128(void), int129(void), int130(void), int131(void), int132(void), 
int133(void);

/* Handlers to be put into the IDT, in order */
typedef void (*interrupt_t) (void);
interrupt_t idt_raw[256] = {

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
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 	NULL, 
NULL, 	NULL, 	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,

/* System Calls */
int128, int129, int130, int131, int132, int133, NULL,   NULL,

};

static void idt_set(uint8_t n, uint32_t base, uint16_t seg, uint8_t flags) {
	if (!base) return; /* Ignore null handlers */
	idt[n].base_l = (uint16_t) (base & 0xFFFF);
	idt[n].base_h = (uint16_t) (base >> 16);
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

void idt_init() {
	extern void idt_flush(void);
	extern void halt(void);
	size_t i;

	memclr(int_handlers, sizeof(handler_t) * 256);
	memclr(idt, sizeof(struct idt_entry) * 256);

	/* Write privileged interrupt handlers (faults, IRQs) */
	for (i = 0; i < 48; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0x8E);
	}

	/* Write usermode interrupt handlers (syscalls) */
	for (i = 96;i< 128; i++) {
		if (idt_raw[i]) idt_set(i, (uint32_t) idt_raw[i], 0x08, 0xEE);
	}

	/* Write the IDT */
	idt_flush(idt);
}
