// Copyright 2009 Nick Johnson

#ifndef SIG_H
#define SIG_H

u32int pass(u32int val, u32int task);	// Pass control to a driver, like an interrupt
u32int sign(u32int sig, u32int task);	// Send an anonymous signal to a task
u32int rsig(u32int sig, u32int hook);	// Register signal handler
u32int fsig(u32int sig);				// Deregister signal handler

// Drivers only
u32int rirq(u32int irq, u32int hook);	// Register IRQ (real or fake) handler

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

#define IRQ(n) (n + 32)

void register_int(u8int n, handler_t handler);

#endif /*SIG_H*/
