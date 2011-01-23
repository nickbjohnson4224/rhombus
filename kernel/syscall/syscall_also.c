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
 * syscall_also (int 0x44)
 *
 * ECX: function
 *
 * Spawns a new thread at the instruction <function>. Returns the thread ID of 
 * the created thread on success, -1 on failure.
 */

struct thread *syscall_also(struct thread *image) {
	struct thread *new_image;
	uint32_t function;

	/* get function pointer */
	function = image->ecx;

	/* create new thread */
	new_image = thread_alloc();
	thread_bind(new_image, image->proc);

	new_image->ds      = 0x23;
	new_image->cs      = 0x1B;
	new_image->ss      = 0x23;
	new_image->eflags  = 0;
	new_image->useresp = new_image->stack + SEGSZ;
	new_image->proc    = image->proc;
	new_image->eip     = function;
	new_image->fxdata  = NULL;
	new_image->user    = image->user;

	/* insert new thread into scheduler */
	schedule_insert(new_image);

	/* return new thread ID in old thread */
	image->eax = new_image->id;

	/* return new thread */
	return new_image;
}
