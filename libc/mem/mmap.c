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

#include <arch.h>
#include <abi.h>

/****************************************************************************
 * mmap
 *
 * Maps memory at the virtual address <addr> of at least size <length>, with
 * permissions indicated by flags in <prot>. (see PROT_*) Returns 0 on
 * success, nonzero on failure.
 */

int mmap(void *addr, size_t length, int prot) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));
	length += (uintptr_t) addr & (PAGESZ - 1);

	if (length % PAGESZ == 0) {
		length /= PAGESZ;
	}
	else {
		length = (length / PAGESZ) + 1;
	}

	return _mmap((uintptr_t) addr, length, prot & (PAGESZ - 1));
}

/****************************************************************************
 * umap
 *
 * Unmaps memory from the virtual address <addr> of at most size <length>.
 * Returns zero on success, nonzero on failure.
 */

int umap(void *addr, size_t length) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));

	if (length % PAGESZ == 0) {
		length /= PAGESZ;
	}
	else {
		length = (length / PAGESZ) + 1;
	}

	return _mmap((uintptr_t) addr, length, MMAP_FREE);
}

/****************************************************************************
 * emap
 *
 * Maps the frame of memory at physical address <frame> to the page at
 * virtual address <addr> with permissions indicated by flags in <prot>.
 * (see PROT_*) Only usable by driver processes. Returns 0 on success, 
 * nonzero on failure.
 */

int emap(void *addr, uint32_t frame, int prot) {
	return _mmap((uint32_t) addr, 1, (frame &~ (PAGESZ - 1)) | prot | MMAP_FRAME);
}

/****************************************************************************
 * phys
 *
 * Returns the physical address of the frame mapped at virtual address 
 * <addr>. <addr> does not have to be page aligned: it is rounded down to the
 * next lowest page.
 */

uintptr_t phys(void *addr) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));

	return _mmap((uint32_t) addr, 0, MMAP_PHYS);
}
