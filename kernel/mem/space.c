/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <string.h>
#include <space.h>
#include <debug.h>
#include <cpu.h>

/****************************************************************************
 * space_alloc
 *
 * Returns a new address space.
 */

space_t space_alloc(void) {
	space_t space = frame_new();
	uint32_t *map = (void*) TMP_SRC;

	/* clear new space */
	page_set(TMP_SRC, page_fmt(space, PF_PRES | PF_RW));
	memclr(map, PAGESZ);

	/* set recursive mapping */
	map[PGE_MAP >> 22] = page_fmt(space, PF_PRES | PF_RW);

	return space;
}

/****************************************************************************
 * space_clone
 *
 * Copies the currently loaded address space so that the new address space 
 * is independent of the old one.
 */

static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg);

space_t space_clone() {
	uint32_t i;
	space_t dest;
	frame_t *exmap, *extbl;

	/* Create new address space */
	dest = space_alloc();

	/* Exmap in new address space */
	space_exmap(dest);
	extbl = (void*) TMP_MAP;
	exmap = (void*) (TMP_MAP + 0x3FF000);

	/* Clone/clear userspace */
	for (i = 0; i < 1023; i++) {
		if (cmap[i] & PF_PRES) {
			if (i >= KSPACE / SEGSZ) {
				exmap[i] = cmap[i];
			}
			else {
				segment_clone(extbl, exmap, i * SEGSZ);
			}
		}
	}

	cpu_flush_tlb_full();

	return dest;
}

/* helper for space_clone */
static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg) {
	uintptr_t i;

	exmap[seg / SEGSZ] = frame_new() | PF_PRES | PF_USER | PF_RW;

	for (i = seg / PAGESZ; i < (seg + SEGSZ) / PAGESZ; i++) {
		extbl[i] = (ctbl[i] & PF_PRES) ?
			(ctbl[i] & PF_LINK) ? ctbl[i] : frame_copy(ctbl[i]) : 0;
	}
}

/****************************************************************************
 * space_exmap
 *
 * Recursively maps an external address space.
 */

void space_exmap(space_t space) {
	cmap[TMP_MAP >> 22] = page_fmt(space, PF_PRES | PF_RW);
	cpu_flush_tlb_full();
}

/****************************************************************************
 * space_free
 *
 * Completely frees resources associated with an address space.
 */

void space_free(space_t space) {
	uint32_t i, j;
	frame_t *extbl, *exmap;

	space_exmap(space);
	extbl = (void*) TMP_MAP;
	exmap = (void*) (TMP_MAP + 0x3FF000);

	for (i = 0; i < KSPACE / SEGSZ; i++) {
		if (exmap[i] & PF_PRES) {
			for (j = 0; j < 1024; j++) {
				if (extbl[i * 1024 + j] & PF_PRES) {
					frame_free(page_ufmt(extbl[i * 1024 + j]));
				}
				extbl[i * 1024 + j] = 0;
			}
			frame_free(page_ufmt(exmap[i]));
			exmap[i] = 0;
		}
	}

	frame_free(space);
}
