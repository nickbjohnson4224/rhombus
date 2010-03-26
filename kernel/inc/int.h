/* Copyright 2009, 2010 Nick Johnson */

#ifndef INT_H
#define INT_H

#include <task.h>

/* Signal structure */
typedef struct signal {
	uint32_t signal;
	uint32_t grant;
	uint32_t caller;
} __attribute__ ((packed)) signal_t;

/* Thread structure - contains the saved state of a task */
typedef struct thread {
	uint32_t fxdata[128];
	uint32_t grant;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t num;
	uint32_t err;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
	uint32_t tss_start;

	struct signal  signal;
	struct process *proc;
} __attribute__ ((packed)) thread_t;

/***** THREAD OPERATIONS *****/
typedef thread_t* (*handler_t) (thread_t*);
thread_t *pit_handler(thread_t *image);

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)
void register_int(uint8_t n, handler_t handler);
void tss_set_esp(uint32_t esp);
void pic_mask(uint16_t mask);

/***** ABI 2 System Calls *****/
thread_t *fire(thread_t *image);
thread_t *drop(thread_t *image);
thread_t *hand(thread_t *image);
thread_t *ctrl(thread_t *image);
thread_t *info(thread_t *image);
thread_t *mmap(thread_t *image);
thread_t *fork(thread_t *image);
thread_t *exit(thread_t *image);

/* Return with eax set to a value */
#define ret(image, value) do { \
image->eax = value; \
return image; \
} while(0);

/***** FAULT HANDLERS *****/
thread_t *fault_generic(thread_t *image);
thread_t *fault_page   (thread_t *image);
thread_t *fault_float  (thread_t *image);
thread_t *fault_double (thread_t *image);

/***** COMMON INTERRUPT HANDLER *****/
thread_t *int_handler(thread_t *image);

#endif /*INT_H*/
