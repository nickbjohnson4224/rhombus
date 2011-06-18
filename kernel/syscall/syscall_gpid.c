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

#include <interrupt.h>
#include <process.h>

/*****************************************************************************
 * syscall_gpid (int 0x4d)
 *
 * ECX: selector
 *
 * Returns information about the current thread and process, depending on the
 * value of <selector>.
 *
 * If <selector> is 0, the current process' pid is returned.
 * If <selector> is 1, the current process' parent's pid is returned.
 * If <selector> is 2, the current thread's tid is returned.
 * If <selector> is 3, the current thread's TLS/stack base is returned.
 * If <selector> is 4, the current thread's uid is returned.
 */

struct thread *syscall_gpid(struct thread *image) {
	
	switch (image->ecx) {
	case 0: image->eax = image->proc->pid; break;
	case 1: image->eax = image->proc->parent->pid; break;
	case 2: image->eax = image->id; break;
	case 3: image->eax = image->stack; break;
	case 4: image->eax = image->user; break;
	}

	return image;
}
