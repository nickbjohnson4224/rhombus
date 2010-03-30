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

struct thread *signal(uint16_t task, uint16_t sig, void* grant, uint8_t flags);
struct thread *sret(struct thread *image);

/***** TASK TABLE *****/

typedef struct process {
	space_t space;
	struct thread *image;
	uint32_t flags;
	uint8_t quanta;
	uint16_t magic;
	pid_t pid;
	struct process *next_task;
	pid_t parent;
	uint32_t shandler;
	uint32_t sigflags;
} task_t, process_t;

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

void            process_init  (void);
struct process *process_get   (pid_t pid);
struct process *process_alloc (void);
struct process *process_clone (struct process *parent);
void            process_free  (struct process *proc);
void            process_kill  (struct process *proc);
void            process_touch (pid_t pid);
struct thread  *process_switch(struct process *proc, uint32_t thread);

extern pid_t curr_pid;		/* Currently loaded task ID */
extern task_t *curr_task;

/***** SCHEDULER *****/

void    sched_ins(pid_t pid);
void    sched_rem(pid_t pid);
task_t *task_next(uint8_t flags);

#endif /*TASK_H*/
