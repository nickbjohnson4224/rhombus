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

#include <interrupt.h>
#include <process.h>
#include <debug.h>

/*****************************************************************************
 * syscall_auth (int 0x4e)
 *
 * ECX: pid
 * EDX: user
 *
 * Changes the user id of the process with pid <pid> to <user>. If <pid> is
 * zero, this changes the effective user id of the current thread to <user>.
 * This syscall is privileged. Returns zero on success, nonzero on error.
 */

struct thread *syscall_auth(struct thread *image) {
	struct process *proc;

	if (image->proc->user != 0) {
		image->eax = -1;
		return image;
	}

	if (image->ecx != 0) {
		proc = process_get(image->ecx);
	
		if (proc) {
			proc->user = image->edx;
			image->user = image->edx;
			image->eax = 0;
		}
		else {
			image->eax = -1;
		}
	}
	else {
		image->user = image->edx;
		image->eax = 0;
	}

	return image;
}
