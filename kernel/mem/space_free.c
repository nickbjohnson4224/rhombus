/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * space_free
 *
 * Completely frees resources associated with an address space.
 */

void space_free(space_t space) {
	uint32_t i, j;
	uintptr_t seg;
	frame_t *extbl, *exmap;

	seg = TMP_MAP;

	space_exmap(seg, space);
	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);

	for (i = 0; i < PGE_MAP / SEGSZ; i++) {
		if (exmap[i] & PF_PRES && !(exmap[i] & SEG_LINK)) {
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
