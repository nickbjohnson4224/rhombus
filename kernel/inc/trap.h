// Copyright 2009 Nick Johnson

#ifndef TRAP_H
#define TRAP_H

typedef struct image {
	uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t num, caller, err, eip, cs, eflags, useresp, ss;
} __attribute__ ((packed)) image_t;

typedef image_t* (*handler_t) (image_t*);
image_t *pit_handler(image_t *state);

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)
void register_int(uint8_t n, handler_t handler);
void tss_set_esp(uint32_t esp);

/***** SYSTEM CALLS *****/
image_t *fork_call(image_t *image);	//0x40
image_t *exit_call(image_t *image);	//0x41
image_t *sint_call(image_t *image);	//0x42
image_t *sret_call(image_t *image);	//0x43
image_t *mmap_call(image_t *image);	//0x44
image_t *umap_call(image_t *image);	//0x45
image_t *rsig_call(image_t *image); //0x46
image_t *lsig_call(image_t *image); //0x47

image_t *mmgc_call(image_t *image); //0x48

image_t *rirq_call(image_t *image); //0x50
image_t *lirq_call(image_t *image); //0x51
image_t *push_call(image_t *image);	//0x52
image_t *pull_call(image_t *image); //0x53
image_t *eout_call(image_t *image); //0x54

#define ret(image, value) do { \
image->eax = value; \
return image; \
} while(0);

/***** FAULT HANDLERS *****/
image_t *fault_generic(image_t *image);
image_t *fault_page(image_t *image);
image_t *fault_float(image_t *image);
image_t *fault_double(image_t *image);

/***** COMMON INTERRUPT HANDLER *****/
void *int_handler(image_t *image);

#endif /*TRAP_H*/
