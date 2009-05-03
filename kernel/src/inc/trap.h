// Copyright 2009 Nick Johnson

#ifndef SIG_H
#define SIG_H

// Signals (system)
#define S_ENT 0x00000001	// Reentrance
#define S_PAG 0x00000002	// Page fault
#define S_IRQ 0x00000004	// Registered IRQ
#define S_KIL 0x00000008	// Kill signal

typedef struct image {
	u32int ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int num, err, eip, cs, eflags, useresp, ss;
} image_t;

typedef void* (*handler_t) (image_t*);

void init_idt();
void init_pit();
void *kb_handler(image_t *state);
void *pit_handler(image_t *state);

#define IRQ(n) (n + 32)
void register_int(u8int n, handler_t handler);

#endif /*SIG_H*/
