/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/memory.h>

bool page_check(space_t space, void *vaddr) {
	frame_t *cur_mirror;
	frame_t *tmp_mirror;
	bool result;
	
	while (!mutex_aquire(m_space, 1));

	if (space == space_curr) {
		tmp_mirror = (void*) SEGMENT_ADDR_MIRROR;
		result = (tmp_mirror[(uintptr_t) vaddr] & FRAME_PRES) ? true : false;
	} else {
		cur_mirror = temp_alloc();
		page_set(KSPACE, cur_mirror, space);

		cur_mirror[SEGMENT_ADDR_TMIRROR / SEGMENT_SIZE] 
			= SEGMENT_SETTYPE(space, SEGMENT_TYPE_KTABLE);
		tmp_mirror = (void*) SEGMENT_ADDR_TMIRROR;
		result = (tmp_mirror[(uintptr_t) vaddr] & FRAME_PRES) ? true : false;

		cur_mirror[SEGMENT_ADDR_TMIRROR / SEGMENT_SIZE] = 0;
		temp_free(cur_mirror);
	}

	mutex_release(m_space, 1);

	return result;
