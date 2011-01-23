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
#include <space.h>

/*****************************************************************************
 * syscall_phys (int 0x47)
 *
 * ECX: address
 *
 * If the caller is privileged, returns the frame of the page of the given
 * virtual address <address> (including permission bits). Otherwise, returns
 * the permission bits of the frame only. <address> must be page aligned, or
 * the result is undefined.
 *
 * This function can be used to look before leaping (and segfaulting) for user
 * processes, and can be used to simplify DMA for device drivers.
 */

struct thread *syscall_phys(struct thread *image) {
	uintptr_t address;

	address = image->ecx;

	/* enforce alignment */
	if (address & 0xFFF) {
		image->eax = 0;
		return image;
	}

	if (image->user) {
		/* not privileged, mask address */
		image->eax  = page_get(address);
		image->eax &= ~(PF_PRES | PF_RW | PF_USER);
	}
	else {
		image->eax = page_get(address);
	}

	return image;
}
