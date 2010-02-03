/* Copyright 2009 Nick Johnson */

#include <config.h>
#include <_libc.h>

int32_t __mmap(uint32_t addr, uint32_t size, uint32_t flags) {
	
	size += addr & 0xFFF;
	addr &= 0xFFFFF000;

	if (size % PAGESZ == 0) {
		size /= PAGESZ;
	}
	else {
		size = (size / PAGESZ) + 1;
	}

	return _mmap(addr, size, flags & 0xF);
}

int32_t __umap(uint32_t addr, uint32_t size) {

	addr &= 0xFFFFF000;

	if (size % PAGESZ == 0) {
		size /= PAGESZ;
	}
	else {
		size = (size / 0x1000) + 1;
	}

	return _mmap(addr, size, MMAP_FREE);
}

int32_t __emap(uint32_t addr, uint32_t frame, uint32_t flags) {

	addr &= 0xFFFFF000;

	return _mmap(addr, 1, (frame &~ 0xFFF) | flags | MMAP_FRAME);
}

uint32_t __pmap(uint32_t addr, uint32_t flags) {
	
	addr &= 0xFFFFF000;

	return _mmap(addr, 1, flags | MMAP_PHYS);
}
