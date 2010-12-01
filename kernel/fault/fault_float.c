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

#include <thread.h>
#include <debug.h>
#include <space.h>
#include <cpu.h>

/*****************************************************************************
 * fault_float
 *
 * Floating point exception handler. If the fault is from userspace (which it
 * always should be...) it freezes the current process and sends it a message
 * on port PORT_FLOAT. If the fault is in kernelspace, it panics.
 */

struct thread *fault_float(struct thread *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		debug_printf("ip = %x\n", image->eip);
		debug_panic("floating point exception");
	}

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_FLOAT, NULL, 0);
}
