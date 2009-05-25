// Copyright 2009 Nick Johnson

#ifndef TRAP_H
#define TRAP_H

typedef struct image {
	u32int ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int num, caller, err, eip, cs, eflags, useresp, ss;
} __attribute__ ((packed)) image_t;

typedef image_t* (*handler_t) (image_t*);
image_t *pit_handler(image_t *state);

#define IRQ(n) (n + 32)
void register_int(u8int n, handler_t handler);
void tss_set_esp();

/***** SYSTEM CALLS *****/
image_t *fork_call(image_t *image);
image_t *exit_call(image_t *image);
image_t *sint_call(image_t *image);
image_t *sret_call(image_t *image);
image_t *mmap_call(image_t *image);
image_t *umap_call(image_t *image);

image_t *eout_call(image_t *image);

/***** FAULT HANDLERS *****/
image_t *fault_generic(image_t *image);
image_t *fault_page(image_t *image);
image_t *fault_float(image_t *image);
image_t *fault_double(image_t *image);

#endif /*TRAP_H*/
