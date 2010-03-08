/*
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#ifndef INT_H
#define INT_H

/* Image structure - contains the saved state of a task */
/* Stored in the task image stack (TIS), used during signals and interrupts */
typedef struct image {
	uint32_t fxdata[128];
	uint32_t flags, grant, mg;
	uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t num, caller, err, eip, cs, eflags, useresp, ss;
} __attribute__ ((packed)) image_t;

typedef image_t* (*handler_t) (image_t*);
image_t *pit_handler(image_t *state);

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)
void register_int(uint8_t n, handler_t handler);
void tss_set_esp(uint32_t esp);
void pic_mask(uint16_t mask);

/***** ABI 2 System Calls *****/
image_t *fire(image_t *image);
image_t *drop(image_t *image);
image_t *hand(image_t *image);
image_t *ctrl(image_t *image);
image_t *info(image_t *image);
image_t *mmap(image_t *image);
image_t *fork(image_t *image);
image_t *exit(image_t *image);

/* Return with eax set to a value */
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

#endif /*INT_H*/
