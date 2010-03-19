/* 
 * Copyright 2009 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <mem.h>

ptbl_t *cmap = (void*) (PGE_MAP + 0x3FF000);	/* Current page directory mapping */
page_t *ctbl = (void*) PGE_MAP;					/* Current base page table mapping */
ptbl_t *tmap = (void*) (TMP_MAP + 0x3FF000);	/* Temporary page directory mapping */
page_t *ttbl = (void*) TMP_MAP;					/* Temporary base page table mapping */
uint32_t *tsrc = (void*) TMP_SRC;				/* Some pages of open virtual memory */
uint32_t *tdst = (void*) TMP_DST;				/* Some pages of open virtual memory */

/* Does a full TLB flush - assembly function */
extern void page_flush_full(void);

/* Map a page directory at the temporary position */
void map_temp(map_t map) {
	cmap[TMP_MAP >> 22] = page_fmt(map, (PF_PRES | PF_RW));
	page_flush_full();
}

/* Allocate a new page directory */
map_t map_alloc() {
	map_t map = frame_new();

	/* Allocate and clear new map */
	page_set((uint32_t) tsrc, page_fmt(map, (PF_PRES | PF_RW)));
	pgclr(tsrc);

	/* Set PGE_MAP table recursively */
	tsrc[PGE_MAP >> 22] = page_fmt(map, (PF_PRES | PF_RW));
	return map;
}

/* Free a page directory (does *not* free tables - use map_clean()) */
map_t map_free(map_t map) {
	frame_free(map);
	return map;
}

/* Removes page table containing page if table is empty */
void map_gc(uint32_t page) {
	uint32_t i, empty;

	if (!(cmap[page >> 22] & PF_PRES)) return;

	empty = 1;
	for (i = 0; i < 1024; i++) {
		if (ctbl[(page >> 12) + i] & PF_PRES) {
			empty = 0;
			break;
		}
	}
	if (empty) {
		frame_free(page_ufmt(cmap[i]));
		cmap[i] = 0;
	}
}	

/* Removes all page tables from page directory and frees them */
map_t map_clean(map_t map) {
	uint32_t i, j;

	map_temp(map);
	for (i = 0; i < LSPACE >> 22; i++) if (tmap[i] & PF_PRES) {
		for (j = 0; j < 1024; j++) {
			if (ttbl[i*1024+j] & PF_PRES) {
				frame_free(page_ufmt(ttbl[i*1024+j]));
			}
			ttbl[i*1024+j] = 0;
		}
		frame_free(page_ufmt(tmap[i]));
		tmap[i] = 0;
	}

	return map;
}

/* Copies an entire address space, linking the kernel */
map_t map_clone() {
	uint32_t i, j;
	map_t dest;

	/* Create new map */
	dest = map_alloc();
	map_temp(dest);

	/* Link kernelspace (except for recursive mapping) */
	for (i = KSPACE >> 22; i < TMP_MAP >> 22; i++)
		tmap[i] = cmap[i];

	/* Clone/clear userspace */
	for (i = 0; i < KSPACE; i += 0x400000) if (cmap[i >> 22] & PF_PRES) {
		tmap[i >> 22] = frame_new() | PF_PRES | PF_USER | PF_RW;
		pgclr(&ttbl[i >> 12]);
		for (j = i; j < i + 0x400000; j += 0x1000) if (ctbl[j >> 12] & PF_PRES) {
			ttbl[j >> 12] = frame_new() | (ctbl[j >> 12] & PF_MASK);
			page_flush_full();
			page_set((uint32_t) tsrc, page_fmt(ctbl[j >> 12], PF_PRES | PF_RW));
			page_set((uint32_t) tdst, page_fmt(ttbl[j >> 12], PF_PRES | PF_RW));
			memcpy(tdst, tsrc, 0x1000);
		}
	}

	return dest;
}
