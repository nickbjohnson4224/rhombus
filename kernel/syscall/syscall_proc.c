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
 * syscall_proc (int 0x50)
 *
 * ECX: selector
 * EDX: pid
 * EBX: value
 *
 * Gets or sets information about a given process, depending on the value of
 * <selector>.
 *
 * If <selector> is 0, the given process' parent's pid is returned.
 * If <selector> is 1, the given process' parent's pid is set to <value>.
 * If <selector> is 2, the given process' uid is returned.
 * If <selector> is 3, the given process' uid is set to <value>.
 */

struct thread *syscall_proc(struct thread *image) {
	struct process *proc;

	switch (image->ecx) {
	case 0: // read parent
		proc = process_get(image->edx);

		if (!proc || !proc->parent) {
			image->eax = 0;
			return image;
		}

		image->eax = proc->parent->pid;
		return image;
	case 1: // write parent
		if (image->edx != image->proc->pid) {
			image->eax = 1;
			return image;
		}

		proc = process_get(image->ebx);

		if (!proc) {
			image->eax = 1;
			return image;
		}

		image->proc->parent = proc;
		image->eax = 0;
		return image;
	case 2: // read uid
		proc = process_get(image->edx);

		if (proc) {
			image->eax = proc->user;
		}
		else {
			image->eax = -1;
		}

		return image;
	case 3: // write uid
		if (image->proc->user != 0) {
			image->eax = -1;
			return image;
		}

		proc = process_get(image->edx);

		if (!proc) {
			image->eax = -1;
			return image;
		}

		proc->user = image->ebx;
		image->eax = 0;
		return image;
	}

	image->eax = 1;
	return image;
}
