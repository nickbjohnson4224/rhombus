/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <interrupt.h>
#include <process.h>

/*****************************************************************************
 * syscall_kill (int 0x51)
 *
 * ECX: target
 * EDX: signal
 *
 * Raises a signal for the target process or process group on the specified
 * port. This is fundamentially the same as a send, but has a few key 
 * differences: first, process groups (specified by negative ID numbers for
 * <target>) are valid targets. Second, no packet can be sent.
 *
 * If <target> is positive, the signal is sent to the PID <target>.
 *
 * If <target> is negative, the signal is sent to <target> and all of its
 * offspring (recursively).
 *
 * Returns zero on success, nonzero on error.
 */

static void _signal_subtree(struct thread *image, pid_t root, uint8_t signal);

struct thread *syscall_kill(struct thread *image) {
	int32_t target = image->ecx;
	uint8_t signal = image->edx;

	/* fail if target is zero */
	if (target == 0) {
		image->eax = 1;
		return image;
	}

	if (target > 0) {
		
		/* check existence of target */
		if (!process_get(target)) {
			image->eax = 1;
			return image;
		}

		/* return zero unless otherwise specified */
		image->eax = 0;

		/* send signal */
		return thread_send(image, target, signal, NULL);

	}
	else {

		_signal_subtree(image, -target, signal);

		image->eax = 0;
	}

	return image;
}

static void _signal_subtree(struct thread *image, pid_t root, uint8_t signal) {
	struct process *proc;
	int i;

	proc = process_get(root);

	for (i = 0; i < MAX_PID; i++) {
		if (process_get(i) && process_get(i)->parent && process_get(i)->parent->pid == root) {
			_signal_subtree(image, i, signal);
		}
	}

	process_freeze(proc);
	schedule_insert(thread_send(image, root, signal, NULL));
}
