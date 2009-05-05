// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

map_t *map_alloc(map_t *map) {
	map->pdir = kmalloc(0x1000);
	map->virt = kmalloc(0x1000);
	memclr((u32int*) map->pdir, 0x1000);
	memclr((u32int*) map->virt, 0x1000);
	return map;
}

map_t *map_free(map_t *map) {
	kfree(map->pdir);
	kfree(map->virt);
	return map;
}

map_t *map_clean(map_t *map) {
	u32int i, j;

	for (i = 0; i < 976; i++) if (map->virt[i]) {
		for (j = 0; j < 1024; j++) {
			frame_free(page_ufmt(map->virt[i][j]));
			map->virt[i][j] = 0x00000000;
		}
		kfree(map->virt[i]);
		map->pdir[i] = 0x00000000;
	}

	return map;
}			

map_t *map_clone(map_t *dest, map_t *src, u8int flags) {
	u32int i, j;

	// Create new map
	dest = map_alloc(dest);

	// Clone/clear userspace
	for (i = 0; i < 976; i++) if (src->virt[i]) {
		for (j = 0; j < 1024; j++) if (src->virt[i][j] & PF_PRES) {
			p_alloc(dest, ((i << 10) + j) << 12, src->virt[i][j] & PF_MASK);
			page_set(&kmap, 0xFFFF0000, (src->virt[i][j]  & ~PF_MASK) | PF_PRES | PF_RW);
			page_set(&kmap, 0xFFFF1000, (dest->virt[i][j] & ~PF_MASK) | PF_PRES | PF_RW);
			memcpy( (void*) 0xFFFF1000, (void*) 0xFFFF0000, 0x1000);
			page_set(&kmap, 0xFFFF0000, 0x00000000);
			page_set(&kmap, 0xFFFF1000, 0x00000000);
		}
	}

	// Link kernel/libspace
	for (i = 976; i < 992; i++) if (src->virt[i]) {
		dest->virt[i] = src->virt[i];
		dest->pdir[i] = src->pdir[i];
	}
	for (i = 992; i < 1024; i++) if (kmap.virt[i]) {
		dest->virt[i] = kmap.virt[i];
		dest->pdir[i] = kmap.pdir[i];
	}

	return dest;
}

map_t *map_load(map_t *map) {
	asm volatile ("mov %0, %%cr3" :: "r" (phys_of(&kmap, map->pdir)));
	return map;
}
