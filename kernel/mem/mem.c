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

#include <stdint.h>
#include <space.h>

/****************************************************************************
 * mem_alloc
 *
 * Allocates at least a range of memory with the given flags.
 */

void mem_alloc(uintptr_t base, uintptr_t size, uint16_t flags) {
	uintptr_t i;

	for (i = base & ~0xFFF; i < base + size; i += 0x1000) {
		if ((page_get(i) & PF_PRES) == 0) {
			page_set(i, page_fmt(frame_new(), (flags & PF_MASK) | PF_PRES));
		}
	}
}

/****************************************************************************
 * mem_free
 *
 * Unmaps at most a range of memory.
 */

void mem_free(uintptr_t base, uintptr_t size) {
	uint32_t i;

	for (i = base & ~0xFFF; i < base + size; i += 0x1000) {
		if (page_get(i) & PF_PRES) {
			frame_free(page_ufmt(page_get(i)));
			page_set(i, 0);
		}
	}
}
