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

#include <stdbool.h>
#include <stdint.h>
#include <space.h>
#include <debug.h>

struct frame_struct {
	struct frame_struct *next;
	struct frame_struct *prev;
	uint32_t refc;
	uint32_t addr;
};

/*****************************************************************************
 * out_of_memory
 *
 * False if the real frame allocator is active. This starts as true to force 
 * the kernel to allocate frames without using the heap, which would otherwise
 * cause a catch-22 where the allocator is needed to run the allocator.
 */

bool out_of_memory = true;

/*****************************************************************************
 * frame_list
 *
 * List of freely available frames. Used for quick allocation.
 */

static struct frame_struct *frame_list;

/*****************************************************************************
 * frame_hash
 *
 * Hashtable of allocated frames, hashed by the low eight bits of the frame
 * address (i.e. the frame divided by PAGESZ). Used to look up allocated 
 * frames to change their reference count.
 */

static struct frame_struct *frame_hash[256];

/*****************************************************************************
 * frame_find
 *
 * Find a frame structure in the frame hash by address.
 */

static struct frame_struct *frame_find(frame_t frame) {
	struct frame_struct *list;

	list = frame_hash[(frame / PAGESZ) & 0xFF];
	
	while (list) {
		if (list->addr == frame / PAGESZ) {
			break;
		}
		list = list->next;
	}

	if (!list) {	
		return NULL;
	}
	else {
		return list;
	}
}

/*****************************************************************************
 * frame_pull
 *
 * Remove a frame structure from whatever list it's in.
 */

static void frame_pull(struct frame_struct *fs) {
	
	if (fs->prev) {
		fs->prev->next = fs->next;
	}
	else {
		frame_hash[fs->addr & 0xFF] = fs->next;
	}

	if (fs->next) {
		fs->next->prev = fs->prev;
	}
}

/*****************************************************************************
 * frame_free
 *
 * "Free" a frame by decreasing its reference count. If the reference count
 * falls to zero, the frame is actually put back into the free list.
 */

void frame_free(frame_t frame) {
	struct frame_struct *fs;

	fs = frame_find(frame);
	
	if (!fs) {
		return;
	}

	if (fs->refc == 1) {
		/* actually free */
		frame_pull(fs);
		fs->prev = NULL;
		fs->next = frame_list;

		if (fs->next) {
			fs->next->prev = fs;
		}

		frame_list = fs;

		out_of_memory = false;
	}
	else {
		fs->refc--;
	}
}

/*****************************************************************************
 * frame_new
 *
 * Return a new frame from the allocator with reference count set to 1.
 */

frame_t frame_new(void) {
	extern uint32_t __end;
	static uint32_t oom_pool = BOOT_POOL;
	struct frame_struct *fs;

	if (out_of_memory) {
		/* out of memory, allocate from OOM pool */
		oom_pool -= PAGESZ;

		if (oom_pool + KSPACE < __end) {
			/* out of emergency memory: panic */
			debug_panic("out of memory");
		}

		return oom_pool;
	}

	/* remove from free list */
	fs = frame_list;
	
	if (!fs) {
		/* no memory to allocate! */
		out_of_memory = true;
		return frame_new();
	}

	frame_list = fs->next;

	if (frame_list) {
		fs->prev = NULL;
	}
	
	/* add to frame hash */
	fs->next = frame_hash[fs->addr & 0xFF];
	fs->prev = NULL;

	if (fs->next) {
		fs->next->prev = fs;
	}
	
	frame_hash[fs->addr & 0xFF] = fs;

	/* set reference count to 1 and return address */
	fs->refc = 1;
	return (fs->addr * PAGESZ);
}

/****************************************************************************
 * frame_add
 *
 * Add a new frame to the frame allocator (used only during init). Frames
 * for the lower 4 MB will be ignored. 
 *
 * Note: This function uses the kernel heap, even though the frame allocator
 * is not set up when it is called. The boot pool hack in frame_new takes
 * care of this, allocating permanent frames from the lower 4 MB instead of
 * ones in the real allocator. Once this function is called (i.e. once the
 * allocator contains at least one real frame) the real allocator is 
 * initialized.
 */

void frame_add(frame_t frame) {
	struct frame_struct *fs;

	/* reject frames from below 4 MB */
	if (frame / PAGESZ <= 1024) {
		return;
	}

	/* allocate frame structure */
	fs = heap_alloc(sizeof(struct frame_struct));

	/* initialize and add to free frame list */
	fs->prev   = NULL;
	fs->refc   = 0;
	fs->addr   = frame / PAGESZ;
	fs->next   = frame_list;
	frame_list = fs;
	if (fs->next) {
		fs->next->prev = fs;
	}

	/* activate real allocator */
	out_of_memory = false;
}

/*****************************************************************************
 * frame_ref
 *
 * Increase the reference count of a frame by 1.
 */

void frame_ref(frame_t frame) {
	struct frame_struct *fs;

	fs = frame_find(frame);
	
	if (!fs) {
		return;
	}

	fs->refc++;
}
