#ifndef PROC_H
#define PROC_H

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/memory.h>

/****************************************************************************
 * Process Control Space
 *
 * This structure represents the control space of a process.
 */

struct proc_ctrl {
	uint32_t id;			/* Process identification number */

	uint32_t sched;			/* Scheduling flags */
	uint32_t quanta;		/* Scheduling quanta remaining */

	uint32_t handler;		/* Signal handler */
	uint32_t sflags;		/* Signal block/hold flags */
};

/****************************************************************************
 * Process Structure
 *
 * This structure represents a process. It is guaranteed to be page aligned,
 * and under a page in size.
 */

typedef struct process {
	space_t  space;				/* Process address space */
	struct process *next_proc;	/* Next process in scheduler */
	struct process *parent;		/* Parent process */
	struct proc_ctrl ctrl;		/* Process control space structure */

	uint32_t m_core;			/* Process structure mutex */
	uint32_t m_threads;			/* Thread space mutex */
} proc_t;

/****************************************************************************
 * Process Operations
 */

proc_t *proc_alloc(void);
void    proc_free (proc_t *process);

proc_t *proc_fork(proc_t *parent);
proc_t *proc_exit(proc_t *process);

#endif/*PROC_H*/
