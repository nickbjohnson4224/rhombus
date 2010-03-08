/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/memory.h>

/****************************************************************************
 * space_copy
 *
 * Copies an address space so that it has the same behavior as the original
 * without being able to affect the original.
 */

static frame_t seg_copy(frame_t old_pseg);
static void page_copy  (frame_t dest, frame_t src);

space_t space_copy(space_t old_space) {
	space_t new_space;
	frame_t *old_mirror;
	frame_t *new_mirror;
	size_t i;

	/* allocate virtual memory */
	old_mirror = temp_alloc();
	new_mirror = temp_alloc();

	/* allocate new empty space */
	new_space = space_alloc();

	/* map all spaces */
	page_set(KSPACE, old_mirror, old_space);
	page_set(KSPACE, new_mirror, new_space);

	/* copy segments */
	for (i = 0; i < PAGESZ / sizeof(frame_t); i++) {
		if (!(old_mirror[i] & FRAME_PRES)) {
			continue;
		}

		if (SEGMENT_TYPE(old_mirror[i]) & SEGMENT_FLAG_LINK) {
			new_mirror[i] = old_mirror[i] | SEGMENT_FLAG_COPY;
		} else {
			new_mirror[i] = seg_copy(new_space, old_space, i);
		}
	}

	/* free virtual memory */
	temp_free(old_mirror);
	temp_free(new_mirror);

	return new_space;
}

/* Copy the contents of an entire segment */
static void seg_copy(frame_t old_pseg) {
	frame_t new_pseg;

	frame_t *old_seg;
	frame_t *new_seg;

	new_pseg = frame_alloc();

	old_seg = temp_alloc();
	new_seg = temp_alooc();

	page_set(KSPACE, old_seg, old_pseg);
	page_set(KSPACE, new_seg, new_pseg);

	for (i = 0; i < PAGESZ / sizeof(frame_t); i++) {
		if (old_seg[i] & FRAME_PRES) {
			new_seg[i] = frame_alloc();
			page_copy(new_seg[i], old_seg[i]);
		}
	}

	temp_free(old_seg);
	temp_free(new_seg);

	return new_pseg;
}

/* Copy the contents of a single frame */
static void page_copy(frame_t dest, frame_t src) {
	void *vsrc;
	void *vdst;

	vsrc = temp_alloc();
	vdst = temp_alloc();

	page_set(KSPACE, vsrc, src);
	page_set(KSPACE, vdst, dest);

	memcpy(vdst, vsrc, PAGESZ);
	
	temp_free(vdst);
	temp_free(vsrc);
}
