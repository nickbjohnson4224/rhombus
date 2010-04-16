/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <time.h>
#include <space.h>

/****************************************************************************
 * schedule_queue
 *
 * A singly linked queue of threads used by the scheduler. It is referred to
 * as the scheduling queue.
 */

struct schedule_queue {
	struct thread *out;
	struct thread *in;
} schedule_queue;

/****************************************************************************
 * schedule_insert
 *
 * Adds a thread to the scheduler.
 */

void schedule_insert(struct thread *thread) {
	
	if (!schedule_queue.in) {
		schedule_queue.out = thread;
		schedule_queue.in  = thread;
		thread->next = NULL;
	}
	else {
		schedule_queue.in->next = thread;
		schedule_queue.in = thread;
		thread->next = NULL;
	}

}

/****************************************************************************
 * schedule_remove
 *
 * Removes a thread from the scheduler by searching for it.
 */

void schedule_remove(struct thread *thread) {
	struct thread *temp;

	if (schedule_queue.out == thread) {
		schedule_queue.out = thread->next;
		return;
	}

	for (temp = schedule_queue.out; temp->next; temp = temp->next) {
		if (temp->next == thread) {
			temp->next = thread->next;

			if (schedule_queue.in == thread) {
				schedule_queue.in = temp;
			}
		}
	}
}

/****************************************************************************
 * schedule_next
 *
 * Returns the next thread in the scheduler. This thread is then moved to the
 * back of the scheduling queue.
 */

struct thread *schedule_next(void) {
	struct thread *thread;

	thread = schedule_queue.out;

	if (thread) {
		schedule_queue.out = thread->next;

		if (!thread->next) {
			schedule_queue.in = NULL;
		}
	}
	else {
		return NULL;
	}

	schedule_insert(thread);

	return thread;
}
