/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

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
