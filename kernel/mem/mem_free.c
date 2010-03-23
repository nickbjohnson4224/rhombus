/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

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
