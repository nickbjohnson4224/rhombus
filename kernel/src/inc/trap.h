// Copyright 2009 Nick Johnson

#ifndef SIG_H
#define SIG_H

typedef struct image {
	u32int ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int num, err, eip, cs, eflags, useresp, ss;
} __attribute__ ((packed)) image_t;

typedef image_t* (*handler_t) (image_t*);

void init_idt();
void init_pit();
image_t *pit_handler(image_t *state);

#define IRQ(n) (n + 32)
void register_int(u8int n, handler_t handler);

void init_tss();
void tss_set_esp();

#endif /*SIG_H*/
