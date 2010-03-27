/* Copyright 2009, 2010 Nick Johnson */

#ifndef INT_H
#define INT_H

#include <task.h>

typedef struct thread {

	/* stored continuation */
	uint32_t fxdata[128];
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

	/* signal descriptor */
	uint32_t signal;
	uint32_t grant;
	uint32_t source;

	/* owning process */
	struct process *proc;
	
} __attribute__ ((packed)) thread_t;

void      thread_init(void);
thread_t *thread_switch(thread_t *old, thread_t *new);

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

/***** FAULT HANDLERS *****/
thread_t *fault_generic(thread_t *image);
thread_t *fault_page   (thread_t *image);
thread_t *fault_float  (thread_t *image);
thread_t *fault_double (thread_t *image);

/***** COMMON INTERRUPT HANDLER *****/
thread_t *int_handler(thread_t *image);

#endif /*INT_H*/
