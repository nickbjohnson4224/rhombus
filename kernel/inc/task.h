// Copyright 2009 Nick Johnson

#ifndef TASK_H
#define TASK_H

#include <lib.h>
#include <trap.h>
#include <mem.h>

typedef uint16_t pid_t;

/***** SIGNALS *****/

// Signals (system) (sig < 16) (esi = sig #)
#define S_GEN 0	// Generic fault 	(ebx = fault #, ecx = errcode)
#define S_ENT 1	// Reentrance		(ebx = tick)
#define S_PAG 2	// Page fault		(ebx = address, ecx = flags)
#define S_IRQ 3	// Registered IRQ	(IRQ number)
#define S_KIL 4	// Kill signal		(ebx = identifier)
#define S_IMG 5	// Image overflow	()
#define S_FPE 6 // FP exception		(ebx = eip)
#define S_DTH 7 // Child death		(eax = child pid, ebx = exit value)

/* This is the most complex, and most important, system call. It sends signal
type to task task with various arguments and is controlled by flags. Arguments
to this function are in the order shown as registers */

image_t *signal(uint16_t task, uint8_t sig, 
	uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags);
image_t *sret();

// The signal table is mapped in all address spaces
extern uint32_t *signal_table;
extern uint32_t *signal_map;
#define SF_SYS 2
#define SF_USE 1
#define SF_NIL 0

/***** TASK TABLE *****/
// Size must be a divisor of 4096
typedef struct {
	map_t map;
	image_t *image;
	uint32_t tss_esp;
	uint8_t flags;
	uint8_t quanta;
	uint16_t magic;
	pid_t pid;
	pid_t next_task;
	pid_t parent;
	pid_t caller;
} task_t;

#define TF_READY 0x00
#define TF_BLOCK 0x01
#define TF_UNBLK 0x02
#define TF_NOERR 0x04
#define TF_EKILL 0x08
#define TF_SUPER 0x10

task_t *task_get(pid_t pid);
task_t *task_new(task_t *src);
uint32_t task_rem(task_t *t);
image_t *task_switch(task_t *t);

extern pool_t tpool[(MAX_TASKS/1024) + 1];	// Pool allocator for task structures
extern task_t *task; 						// Array of task structures
extern pid_t curr_pid;						// Currently loaded task ID

/***** IRQ REDIRECTION *****/

extern pid_t irq_holder[15];

/***** SCHEDULER *****/

extern struct sched_queue {
	pid_t next;
	pid_t last;
} queue;

void insert_sched(pid_t pid);
void remove_sched(pid_t pid);
task_t *next_task(uint8_t flags);

#define SF_FORCED 0x00
#define SF_VOLUNT 0x01

#endif /*TASK_H*/
