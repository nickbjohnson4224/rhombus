/* Copyright 2009, 2010 Nick Johnson */

#ifndef TASK_H
#define TASK_H

#include <lib.h>
#include <int.h>
#include <mem.h>

typedef uint16_t pid_t;

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

image_t *signal(uint16_t task, uint16_t sig, void* grant, uint8_t flags);
image_t *sret(image_t *image);

/***** TASK TABLE *****/

typedef struct task {
	map_t map;
	image_t *image;
	uint32_t flags;
	uint8_t quanta;
	uint16_t magic;
	pid_t pid;
	struct task *next_task;
	pid_t parent;
	uint32_t shandler;
	uint32_t grant;
	uint32_t sigflags;
} task_t;

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

void     task_touch(pid_t pid);
task_t  *task_get(pid_t pid);
task_t  *task_new(task_t *src);
uint32_t task_rem(task_t *t);
image_t *task_switch(task_t *t);

extern pool_t *tpool;		/* Pool allocator for task structures */
extern task_t *task; 		/* Array of task structures */
extern pid_t curr_pid;		/* Currently loaded task ID */
extern task_t *curr_task;	/* Currently loaded task pointer */

/***** IRQ REDIRECTION *****/

extern pid_t irq_holder[15];

/***** SCHEDULER *****/

void sched_ins(pid_t pid);
void sched_rem(pid_t pid);
task_t *task_next(uint8_t flags);

#define SF_FORCED 0x00
#define SF_VOLUNT 0x01

#endif /*TASK_H*/
