/* 
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <thread.h>
#include <debug.h>

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

	if (!schedule_queue.out && schedule_queue.in) {
		debug_printf("scheduler inconsistency\n");
		debug_panic("");
	}

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

	if (!schedule_queue.out) {
		debug_printf("scheduler empty, but tried to remove %x\n", thread);
		debug_panic("");
		return;
	}

	if (schedule_queue.out == thread) {
		schedule_queue.out = thread->next;
		if (schedule_queue.in == thread) {
			schedule_queue.in = NULL;
		}
	}
	else for (temp = schedule_queue.out; temp->next; temp = temp->next) {
		if (temp->next == thread) {
			temp->next = thread->next;

			if (schedule_queue.in == thread) {
				schedule_queue.in = temp;
			}

			thread->next = NULL;

			break;
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

	do {
		if (thread) {
			schedule_queue.out = thread->next;

			if (!thread->next) {
				schedule_queue.in = NULL;
			}
		}
		else {
			return NULL;
		}

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
	} while (thread->frozen);		

	return thread;
}
