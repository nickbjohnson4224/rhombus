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
 * fault_nomath
 *
 * Math coprocessor total existence failure. This is (or will be) mostly used
 * for dynamic floating point state loading (with the EM and TS flags.) For
 * now, it acts just like the general fault.
 */

struct thread *fault_nomath(struct thread *image) {
	extern uint32_t can_use_fpu;

	if (!can_use_fpu) {
		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_ILL);
	}

	return image;
}
