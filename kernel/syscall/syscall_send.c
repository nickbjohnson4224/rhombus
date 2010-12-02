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

#include <process.h>
#include <thread.h>
#include <debug.h>

/*****************************************************************************
 * syscall_send (int 0x40)
 *
 * EBX: base
 * ECX: count
 * EDX: port
 * ESI: target
 *
 * Sends a message to the port <port> of the process with pid <target>. If 
 * that process does not exist, the call is rejected. If <count> is nonzero, a
 * packet of <count> pages is created from the memory region at <base>, and 
 * unmapped from the current process. If the region is not entirely present, 
 * the call is rejected. The receiving process will have the following register 
 * contents:
 *
 * ECX: count
 * EDX: port
 * ESI: source
 *
 * <count> is the size of the attached packet in pages (zero means no packet);
 * <port> is the requested target port; source is the pid of the sender. The 
 * receiving thread will have the effective user id of the sending thread if the 
 * receiving process has user id 0, and will otherwise have the user id of the 
 * receiving process.
 * 
 * Returns zero on success, nonzero on failure. If <target> is zero, no
 * message is sent, and the current timeslice is relinquished.
 */

struct thread *syscall_send(struct thread *image) {
	uintptr_t base   = image->ebx;
	uintptr_t count  = image->ecx;
	uintptr_t port   = image->edx;
	uintptr_t target = image->esi;
	uintptr_t i;
	struct msg *message;

	/* relinquish timeslice if <target> is zero */
	if (target == 0) {
		image->eax = 0;
		return schedule_next();
	}

	/* check existence of <target> */
	if (!process_get(target)) {
		/* process does not exist */
		image->eax = 1;
		return image;
	}

	/* create message if <count> is nonzero */
	if (count) {
		
		/* check alignment of region */
		if (base & 0xFFF) {
			image->eax = 1;
			return image;
		}

		/* bounds check region */
		if (base >= KSPACE || base + (count * PAGESZ) >= KSPACE) {
			image->eax = 1;
			return image;
		}

		/* reject insane requests (> 64MB) */
		if (count > 0x4000) {
			image->eax = 1;
			return image;
		}

		/* verify continuity of region */
		for (i = 0; i < count; i++) {
			if ((page_get(base + i * PAGESZ) & PF_PRES) == 0) {
				image->eax = 1;
				return image;
			}
		}

		/* allocate message structure */
		message = heap_alloc(sizeof(struct msg));
		message->count = count;
		message->frame = heap_alloc(count * sizeof(uint32_t));

		/* move frames to message */
		for (i = 0; i < count; i++) {
			message->frame[i] = page_get(base + i * PAGESZ);
			page_set(base + i * PAGESZ, 0);
		}

	}
	else {
		message = NULL;
	}

	/* send message */
	return thread_send(image, target, port, message);
}
