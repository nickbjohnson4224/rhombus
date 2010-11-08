/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <process.h>
#include <thread.h>

/*****************************************************************************
 * syscall_send (int 0x40)
 *
 * EDX:	port
 * ECX: target
 *
 * Sends a message to the port <port> of the process with pid <target>.
 * Returns zero on success, nonzero on failure. If <target> is zero, no
 * message is sent, and the current timeslice is relinquished.
 */

struct thread *syscall_send(struct thread *image) {
	pid_t    target = image->ecx;
	portid_t port   = image->edx;

	if (target == 0) {
		image->eax = 0;
		return schedule_next();
	}

	if (!process_get(target)) {
		image->eax = -1;
		return image;
	}
	else {
		image->eax = 0;
	}

	return thread_send(image, target, port);
}
