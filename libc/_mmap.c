#include "libc.h"

int32_t __mmap(uint32_t addr, uint32_t size, uint32_t flags) {
	
	size += addr & 0xFFF;
	addr &= 0xFFFFF000;

	if (size % 0x1000 == 0) {
		size /= 0x1000;
	}
	else {
		size = (size / 0x1000) + 1;
	}

	return _mmap(addr, size, flags);
}

int32_t __umap(uint32_t addr, uint32_t size) {

	addr &= 0xFFFFF000;

	if (size % 0x1000 == 0) {
		size /= 0x1000;
	}
	else {
		size = (size / 0x1000) + 1;
	}

	return _mmap(addr, size, MMAP_FREE);
}

int32_t __emap(uint32_t addr, uint32_t frame, uint32_t flags) {

	addr &= 0xFFFFF000;

	return _mmap(addr, 0, frame | flags | MMAP_GRANT);
}
