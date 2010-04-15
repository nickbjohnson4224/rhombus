/* Copyright 2010 Nick Johnson */

#ifndef TIME_H
#define TIME_H

#include <flux/arch.h>
#include <space.h>

/***** SIGNALS *****/

/* Signals (system) (sig < 16) */
#define SSIG_FAULT	0	/* Generic fault */
#define SSIG_ENTER	1	/* Reentrance */
#define SSIG_PAGE	2	/* Page fault */
#define SSIG_IRQ	3	/* Registered IRQ */
#define SSIG_KILL	4	/* System kill signal */
#define SSIG_IMAGE	5	/* Image overflow */
#define SSIG_FLOAT	6	/* FPU exception */
#define SSIG_DEATH	7	/* Child death */

#define UNBLK 0x01
#define NOERR 0x02
#define EKILL 0x04

struct signal_queue {
	struct signal_queue *next;
	uint32_t signal;
	uint32_t grant;
	uint32_t source;
};

/***** PROCESSES *****/

typedef uint16_t pid_t;

typedef struct process {

	/* address space */
	space_t space;

	/* thread image stack */
	struct thread *image;

	/* various crap */
	uint32_t flags;
	uint32_t pid;
	uint32_t shandler;

	struct process *next_task;
	struct process *parent;

	/* signal policy */
	uint32_t signal_policy[32];
	uint32_t signal_handler;

	/* signal queue */
	struct signal_queue *mailbox_in [32];
	struct signal_queue *mailbox_out[32];

} task_t, process_t;

void            process_init  (void);
struct process *process_get   (pid_t pid);
struct process *process_alloc (void);
struct process *process_clone (struct process *parent);
void            process_free  (struct process *proc);
void            process_kill  (struct process *proc);
void            process_touch (pid_t pid);
struct thread  *process_switch(struct process *proc, uint32_t thread);

/***** CONTROL SPACE *****/

#define CTRL_PSPACE	0
#define CTRL_SSPACE 1

#define CTRL_CMASK	0xFF8000FF
#define CTRL_SMASK 	0x00773047
#define CTRL_RMASK	0x00000003

#define CTRL_READY 	0x00000000
#define CTRL_BLOCK 	0x00000001
#define CTRL_CLEAR 	0x00000002
#define CTRL_ENTER 	0x00000004
#define CTRL_SUPER 	0x00000008
#define CTRL_PORTS 	0x00000010
#define CTRL_IRQRD 	0x00000020
#define CTRL_FLOAT 	0x00000040
#define CTRL_RNICE 	0x00000080
#define CTRL_QUEUE	0x00400000
#define CTRL_MMCLR	0x00800000

#define CTRL_SFAULT	0x00000001
#define CTRL_SENTER	0x00000002
#define CTRL_SPAGE	0x00000004
#define CTRL_SIRQ	0x00000008
#define CTRL_SKILL	0x00000010
#define CTRL_SIMAGE	0x00000020
#define CTRL_SFLOAT	0x00000040
#define CTRL_SDEATH	0x00000080

extern pid_t curr_pid;		/* Currently loaded task ID */
extern task_t *curr_task;

/***** SCHEDULER *****/

void    sched_ins(pid_t pid);
void    sched_rem(pid_t pid);
task_t *task_next(uint8_t flags);

/***** THREADS ******/

typedef struct thread {

	/* stored continuation */
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
	uint32_t *fxdata;
	
	/* signal descriptor */
	uint32_t signal;
	uint32_t grant;
	uint32_t source;

	/* owning process */
	struct process *proc;

	/* user stack segment base */
	uintptr_t stack;

	/* (legacy) thread image stack */
	struct thread *tis;
	
} __attribute__ ((packed)) thread_t;

void      thread_init  (void);
thread_t *thread_alloc (void);
void      thread_free  (thread_t *thread);
thread_t *thread_switch(thread_t *old, thread_t *new);
thread_t *thread_fire(thread_t *image, uint16_t targ, uint16_t sig, uintptr_t grant);
thread_t *thread_drop  (thread_t *image);

/***** SYSTEM CALLS AND OTHER INTERRUPTS *****/

typedef thread_t* (*handler_t) (thread_t*);
thread_t *pit_handler(thread_t *image);

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)
void register_int(uint8_t n, handler_t handler);
void tss_set_esp(uint32_t esp);
void pic_mask(uint16_t mask);

thread_t *syscall_fire(thread_t *image); /* send signals / create threads */
thread_t *syscall_drop(thread_t *image); /* exit from threads */
thread_t *syscall_sctl(thread_t *image); /* query signal policy */
thread_t *syscall_mail(thread_t *image); /* recieve signals */

thread_t *syscall_fork(thread_t *image); /* create processes */
thread_t *syscall_exit(thread_t *image); /* exit from processes */
thread_t *syscall_pctl(thread_t *image); /* query process metadata */
thread_t *syscall_kctl(thread_t *image); /* query kernel metadata */

thread_t *syscall_mmap(thread_t *image); /* manage memory */
thread_t *syscall_mctl(thread_t *image); /* query memory subsystem */

thread_t *fault_generic(thread_t *image);
thread_t *fault_page   (thread_t *image);
thread_t *fault_float  (thread_t *image);
thread_t *fault_double (thread_t *image);
thread_t *fault_nomath (thread_t *image);

thread_t *int_handler(thread_t *image);

#endif
