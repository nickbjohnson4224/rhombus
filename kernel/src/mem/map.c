// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

ptbl_t *tmap = (void*) 0xFFBFF000;
page_t *ttbl = (void*) 0xFF800000;
u32int *tsrc = (void*) 0xFF7FF000;
u32int *tdst = (void*) 0xFF7FE000;

static void map_temp(map_t map) {
	u32int i, t;

	cmap[0x7FE] = map | (PF_PRES | PF_RW);
	for (i = 0; i < 1024; i++) {
		t = (u32int) ttbl + i;
		asm volatile ("invlpg %0" :: "m" (t));
	}
}

map_t map_alloc(map_t map) {
	map = frame_alloc();
	map_temp(map);
	pgclr(tmap);
	return map;
}

map_t map_free(map_t map) {
	frame_free(map);
	return map;
}

map_t map_clean(map_t map) {
	u32int i, j;

	map_temp(map);
	for (i = 0; i < 992; i++) if (tmap[i] & PF_PRES) {
		for (j = 0; j < 1024; j++) {
			frame_free(page_ufmt(ttbl[i*1024+j]));
			ttbl[i*1024+j] = 0;
		}
		frame_free(page_ufmt(tmap[i]));
		tmap[i] = 0;
	}

	return map;
}

map_t map_clone(u8int flags) {
	u32int i, j;
	map_t dest;

	// Create new map
	dest = map_alloc(dest);
	map_temp(dest);

	// Clone/clear userspace
	for (i = 0; i < 0xF8000; i++) if (cmap[i >> 10] & PF_PRES && ctbl[i] & PF_PRES) {
		if (tmap[i >> 10] & PF_PRES == 0) tmap[i >> 10] = frame_new() | 0x7;
		ttbl[i] = frame_new() | (ctbl[i] & PF_MASK);
		page_set(tsrc, page_fmt(ctbl[i], PF_PRES | PF_RW));
		page_set(tdst, page_fmt(ttbl[i], PF_PRES | PF_RW));
		memcpy(tdst, tsrc, 0x1000);
	}

	// Link kernel/libspace
	for (i = 992; i < 1024; i++) if (cmap[i] & PF_PRES)
		tmap[i] = cmap[i];

	return dest;
}

map_t map_load(map_t map) {
	asm volatile ("mov %0, %%cr3" :: "r" (map));
	return map;
}
