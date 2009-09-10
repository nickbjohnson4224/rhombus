/* Copyright 2009 Nick Johnson */

#ifndef TRAP_H
#define TRAP_H

typedef struct image {
/*	uint32_t fxdata[128];  */
/*	uint32_t fxpadding[3]; */
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
image_t *fork_call(image_t *image);	/* 0x40 - fork */
image_t *exit_call(image_t *image);	/* 0x41 - exit */
image_t *gpid_call(image_t *image); /* 0x42 - get pid */
image_t *tblk_call(image_t *image); /* 0x43 - task block */
image_t *mmap_call(image_t *image);	/* 0x44 - memory map */
image_t *umap_call(image_t *image);	/* 0x45 - memory unmap */
image_t *ssnd_call(image_t *image);	/* 0x46 - signal send */
image_t *sret_call(image_t *image);	/* 0x47 - signal return */
image_t *sblk_call(image_t *image); /* 0x48 - signal block */
image_t *sreg_call(image_t *image); /* 0x49 - signal register */

image_t *ireg_call(image_t *image); /* 0x50 - interrupt register */
image_t *irel_call(image_t *image); /* 0x51 - interrupt release */
image_t *push_call(image_t *image);	/* 0x52 - remote push */
image_t *pull_call(image_t *image); /* 0x53 - remote pull */
image_t *eout_call(image_t *image); /* 0x54 - emergency output */
image_t *drop_call(image_t *image); /* 0x55 - drop from drivermode */

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
