#ifndef SIGNAL_H
#define SIGNAL_H

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/memory.h>

/****************************************************************************
 * Signal Structure
 *
 * This structure represents a signal about to be fired to a process. It is
 * created by the function signal_make, which interprets a calling threads'
 * registers to produce a signal based on the system call interface.
 */

typedef struct signal {
	uint32_t signal;		/* Signal number */
	uint32_t target;		/* Target process number */
	uint32_t trans;			/* Transaction number */
	frame_t  request;		/* Physical address of request */
} signal_t;

signal_t *signal_make(struct thread *state);

/* Signal queue management */
signal_t *signal_push(proc_t *process, signal_t *signal);
signal_t *signal_pull(proc_t *process);

struct thread *signal_fire(struct thread *state, signal_t *signal);
struct thread *signal_drop(struct thread *state, signal_t *signal);

#endif/*SIGNAL_H*/
