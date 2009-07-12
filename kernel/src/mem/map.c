// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

ptbl_t *cmap = (void*) 0xFFFFF000;
page_t *ctbl = (void*) 0xFFC00000;
ptbl_t *tmap = (void*) 0xFFBFF000;
page_t *ttbl = (void*) 0xFF800000;
u32int *tsrc = (void*) 0xFF7FF000;
u32int *tdst = (void*) 0xFF7FE000;

void map_temp(map_t map) {
	u32int i, t;
	cmap[1022] = page_fmt(map, (PF_PRES | PF_RW));
	for (i = 0; i < 1024; i++) {
		t = (u32int) ttbl + i;
		asm volatile ("invlpg %0" :: "m" (t));
	}
}

map_t map_alloc(map_t map) {
	map = frame_new();
	page_set((u32int) tsrc, page_fmt(map, (PF_PRES | PF_RW)));
	pgclr(tsrc);
	tsrc[1023] = page_fmt(map, (PF_PRES | PF_RW));
	map_temp(map);
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

map_t map_clone() {
	u32int i, j;
	map_t dest;

	// Create new map
	dest = map_alloc(dest);

	// Clone/clear userspace
	for (i = 0; i < 0xF8000000; i += 0x400000) if (cmap[i >> 22] & PF_PRES) {
		tmap[i >> 22] = frame_new() | PF_PRES | PF_USER | PF_RW;
		for (j = i; j < i + 0x400000; j += 0x1000) if (ctbl[j >> 12] & PF_PRES) {
			ttbl[j >> 12] = frame_new() | (ctbl[j >> 12] & PF_MASK);
			page_set((u32int) tsrc, page_fmt(ctbl[j >> 12], PF_PRES | PF_RW));
			page_set((u32int) tdst, page_fmt(ttbl[j >> 12], PF_PRES | PF_RW));
			memcpy(tdst, tsrc, 0x1000);
		}
	}

	// Link kernel/libspace (except for recursive mapping)
	for (i = 992; i < 1022; i++) if (cmap[i] & PF_PRES)
		tmap[i] = cmap[i];

	return dest;
}

map_t map_load(map_t map) {
	asm volatile ("mov %0, %%cr3" :: "r" (map));
	return map;
}
