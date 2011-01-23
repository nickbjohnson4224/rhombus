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
#include <debug.h>

/*****************************************************************************
 * syscall_stop (int 0x4a)
 *
 * ECX: tid
 *
 * Blocks the thread with ID <tid>. If <tid> is -1, all threads except the
 * current one are blocked.
 */

struct thread *syscall_stop(struct thread *image) {
	struct thread *thread;
	uint32_t tid;

	tid = image->ecx;

	if (tid == (uint32_t) -1) {
		process_freeze(image->proc);
		thread_thaw(image);
		return image;
	}

	if (tid > MAX_THREADS) {
		return image;
	}

	thread = image->proc->thread[tid];

	if (!thread) {
		return image;
	}
	else {
		thread_freeze(thread);
		return image;
	}
}
