/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <mem.h>

/* Page frame allocation pool */
pool_t *fpool;

/* Map a range of pages with given flags */
void mem_alloc(uintptr_t base, uintptr_t size, uint16_t flags) {
	uint32_t i;

	for (i = base & ~0xFFF; i < base + size; i += 0x1000)
		if ((page_get(i) & PF_PRES) == 0)
			p_alloc(i, flags);

}

/* Unmap a range of pages */
void mem_free(uintptr_t base, uintptr_t size) {
	uint32_t i;

	for (i = base & ~0xFFF; i < base + size; i += 0x1000)
		if (page_get(i) & PF_PRES)
			p_free(i);
}	

/* Make sure table exists for a page */
void page_touch(uint32_t page) {
	page &= ~0x3FFFFF;
	if (cmap[page >> 22] & PF_PRES) return;
	cmap[page >> 22] = frame_new() | (PF_PRES | PF_RW | PF_USER);
	page_flush((uint32_t) &ctbl[page >> 12]);
	pgclr(&ctbl[page >> 12]);
}

/* Set the value of a page table entry */
void page_set(uint32_t page, page_t value) {
	page &= ~0xFFF;
	if ((cmap[page >> 22] & PF_PRES) == 0) page_touch(page);
	ctbl[page >> 12] = value;
	page_flush(page);
}

/* Get the value of a page table entry */
page_t page_get(uint32_t page) {
	return (cmap[page >> 22] & PF_PRES) ? ctbl[page >> 12] : 0;
}

/* Makes sure table exists for a page in the temporary mapping */
void temp_touch(uint32_t page) {
	page &= ~0x3FFFFF;
	if (tmap[page >> 22] & PF_PRES) return;
	tmap[page >> 22] = frame_new() | (PF_PRES | PF_RW | PF_USER);
	page_flush((uint32_t) &ttbl[page >> 12]);
	pgclr(&ctbl[page >> 12]);
}

/* Set the value of a page table entry in the temporary mapping */
void temp_set(uint32_t page, page_t value) {
	page &= ~0xFFF;
	if ((tmap[page >> 22] & PF_PRES) == 0) temp_touch(page);
	ttbl[page >> 12] = value;
	page_flush(page);
}

/* Get the value of a page table entry in the temporary mapping */
page_t temp_get(uint32_t page) {
	return (tmap[page >> 22] & PF_PRES) ? ttbl[page >> 12] : 0;
}
