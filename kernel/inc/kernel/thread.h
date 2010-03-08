#ifndef THREAD_H
#define THREAD_H

#include <flux/arch.h>
#include <kernel/proc.h>
#include <kernel/signal.h>

/****************************************************************************
 * Thread Control Space
 *
 * This structure contains all of the metadata about a thread that is in some
 * way accessible to userspace. It must be accessible as an array of 64
 * register-sized elements.
 */

struct thread_ctrl {
	uint32_t id;		/* Thread identification number */
	uint32_t sched;		/* Scheduling flags */
	uint32_t m_core;	/* Processor core mutex */
};

/****************************************************************************
 * Thread Structure
 *
 * This structure represents the saved state of a single thread, excluding its
 * stack, which is simply pointed to by useresp. It is passed to every high 
 * level interrupt handler, and every high level interrupt handler returns it.
 * It is created by the assembly function thread_save, and reloaded by the
 * assembly function thread_load, both in the file interrupt.s. It is 
 * guaranteed to be page aligned and under a page in size, even though it does 
 * not have to be for the machine to work properly: this is for allocation 
 * reasons.
 */

typedef struct thread {
	uint8_t  fxdata[512]; 	/* MMX/SSE/FPU state */
	uint32_t ds;			/* Saved data segment */
	uint32_t edi;        	/* Saved EDI */
	uint32_t esi;			/* Saved ESI */
	uint32_t ebp;			/* Saved base pointer */
	uint32_t esp;			/* Saved stack pointer (bogus) */
	uint32_t ebx;			/* Saved EBX */
	uint32_t edx;			/* Saved EDX */
	uint32_t ecx;			/* Saved ECX */
	uint32_t eax;			/* Saved EAX */
	uint32_t num;			/* Interrupt number */
	uint32_t err;			/* Error code */
	uint32_t eip;			/* Saved instruction pointer */
	uint32_t cs;			/* Saved code segment */
	uint32_t eflags;		/* Saved EFLAGS */
	uint32_t stack;			/* Saved stack pointer */
	uint32_t ss;			/* Saved stack segment */
	uint32_t tss_start;		/* TSS points to &<thread>.tss_start */

	struct process *proc;	/* Owning process structure */
	signal_t signal;		/* Copy of spawning signal structure */

	struct thread_ctrl ctrl;/* Thread control space structure */
} __attribute__ ((packed)) thread_t;

#define THREAD_INISTACK 0x10000

/****************************************************************************
 * Thread Operations
 */

thread_t *thread_spawn (proc_t *process);
void      thread_kill  (thread_t *thread);
thread_t *thread_switch(thread_t *thread, thread_t *old);

extern void thread_load(void);
extern void thread_save(void);

/****************************************************************************
 * Interrupts
 */

void idt_init();
void idt_flush(void *idt);

void pic_mask(uint16_t mask);

typedef thread_t* (*int_handler_t) (thread_t*);
int_handler_t int_reg_handler[256];

/****************************************************************************
 * Task State Segment
 */

void tss_init   (void);
void tss_set_esp(uintptr_t esp);

/****************************************************************************
 * FPU Operations
 */

uint32_t  fpu_p;
void      fpu_init(void);
thread_t *fpu_save(thread_t *state);
thread_t *fpu_load(thread_t *state);

/****************************************************************************
 * Mutexes
 *
 * Mutexes are 32 bit integers used for concurrent synchronization. They can
 * be "aquired" only one time, and keep a key with which to release them in
 * order to ensure safety. The key 0 is reserved for the unlocked value;
 */

bool mutex_aquire (uint32_t *mutex, uint32_t key);
bool mutex_release(uint32_t *mutex, uint32_t key);

#define MUTEX_OPEN 0

#endif/*THREAD_H*/
