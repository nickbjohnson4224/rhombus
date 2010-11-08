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
#include <thread.h>
#include <debug.h>

/****************************************************************************
 * syscall_exit (int 0x49)
 *
 * (no arguments)
 *
 * Exits the current process, switching to init temporarily, then switching
 * to the next schedulable thread. If init calls this syscall, the kernel
 * halts. Does not return on success.
 */

struct thread *syscall_exit(struct thread *image) {

	if (image->proc->pid == 1) {
		debug_panic("init died");
	}

	process_switch(process_get(1));
	process_kill(image->proc);

	return thread_switch(image, schedule_next());
}
