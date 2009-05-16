// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

u8int mem_setup = 0;

page_t page_touch(map_t *map, u32int page) {
	if (map->virt[page >> 22]) return map->virt[page >> 22][(page >> 12) % 1024];
	map->virt[page >> 22] = kmalloc(0x1000);
	map->pdir[page >> 22] = phys_of(&kmap, (void*) map->virt[page >> 22]) | 0x7;
	pgclr(map->virt[page >> 22]);
	return 0x00000000;
}

page_t page_set(map_t *map, u32int page, page_t value) {
	if (!map->virt[page >> 22]) page_touch(map, page);
	return (map->virt[page >> 22][(page >> 12) % 1024] = value);
}

page_t page_get(map_t *map, u32int page) {
	if (!map->virt[page >> 22]) return 0x00000000;
	return map->virt[page >> 22][(page >> 12) % 1024];
}

u32int phys_of(map_t *map, void *addr) {
	u32int temp;

	if (!mem_setup) return (u32int) addr & ~0xF8000000;
	if (!map->virt[(u32int) addr >> 22]) return 0x00000000;
	temp = map->virt[(u32int) addr >> 22][((u32int) addr >> 12) % 1024];
	temp &= 0xFFFFF000;
	temp |= ((u32int) addr & 0xFFF);
	return temp;
}

u32int p_alloc(map_t *map, u32int addr, u32int flags) {
	if (page_get(map, addr) & PF_PRES) return 1;
	page_set(map, addr, page_fmt(frame_new(), flags | PF_PRES));
	return 0;
}

u32int p_free (map_t *map, u32int addr) {
	if (!page_get(map, addr) & PF_PRES) return 1;
	frame_free(page_ufmt(page_get(map, addr)));
	page_set(map, addr, 0);
	return 0;
}
