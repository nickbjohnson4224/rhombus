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
#include <space.h>

/*****************************************************************************
 * syscall_svpr (int 0x45)
 *
 * ECX: field
 * EDX: addr
 *
 * Sets a field in the current thread's virtual package register to <addr>.
 * The only accessible field is the packet frame, which is field 0. Returns
 * zero on success, nonzero on failure.
 */

struct thread *syscall_svpr(struct thread *image) {
	uintptr_t addr = image->edx;
	uint32_t field = image->ecx;

	if (field == 0) {
		if (image->packet) {
			if (image->packet->frame) {
				frame_free(image->packet->frame);
			}
		}
		else {
			image->packet = heap_alloc(sizeof(struct packet));
		}

		if (addr) {
			image->packet->frame  = page_get(addr);
			page_set(addr, page_fmt(frame_new(), image->packet->frame));
		}
		else {
			image->packet->frame  = 0;
		}

		image->eax = 0;
		return image;
	}
	
	image->eax = -1;
	return image;
}
