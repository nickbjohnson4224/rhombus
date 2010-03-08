/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/memory.h>
#include <kernel/thread.h>

/****************************************************************************
 * space_alloc
 *
 * Allocates a new address space structure, clears it, and makes it suitable
 * for loading.
 */

space_t space_alloc() {
	space_t new_space;
	frame_t *temp_mapping;

	/* allocate new space */
	new_space = frame_alloc();

	/* make temporary mapping */
	temp_mapping = temp_alloc();
	page_set(KSPACE, temp_mapping, new_space);
	page_flush(temp_mapping);

	/* clear space */
	memclr(temp_mapping, PAGESZ);

	/* setup mirror */
	temp_mapping[SEGMENT_ADDR_MIRROR / PAGESZ] = new_space;

	/* free temporary mapping */
	temp_free(temp_mapping);

	return new_space;
}
