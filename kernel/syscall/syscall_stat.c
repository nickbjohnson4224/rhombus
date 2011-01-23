/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <timer.h>

/*****************************************************************************
 * syscall_stat (int 0x45)
 *
 * ECX: tid
 *
 * Returns the state of the thread with id <tid>. If the thread still exists, 
 * the return value is 1; otherwise, it is 0.
 */

struct thread *syscall_stat(struct thread *image) {
	struct thread *thread;
	uint32_t tid;

	tid = image->ecx;

	if (tid > MAX_THREADS) {
		image->eax = 0;
		return image;
	}

	thread = image->proc->thread[tid];

	if (!thread) {
		image->eax = 0;
		return image;
	}
	else {
		image->eax = 1;
		return image;
	}
}
