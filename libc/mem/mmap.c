/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <arch.h>
#include <abi.h>

int mmap(void *addr, size_t length, int prot) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));
	length += (uintptr_t) addr & (PAGESZ - 1);

	if (length % PAGESZ == 0) {
		length /= PAGESZ;
	}
	else {
		length = (length / PAGESZ) + 1;
	}

	return _mmap((uintptr_t) addr, length, prot & (PAGESZ - 1));
}

int umap(void *addr, size_t length) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));

	if (length % PAGESZ == 0) {
		length /= PAGESZ;
	}
	else {
		length = (length / PAGESZ) + 1;
	}

	return _mmap((uintptr_t) addr, length, MMAP_FREE);
}

int emap(void *addr, uint32_t frame, int prot) {
	return _mmap((uint32_t) addr, 1, (frame &~ (PAGESZ - 1)) | prot | MMAP_FRAME);
}

uintptr_t phys(void *addr) {
	addr = (void*) ((uintptr_t) addr &~ (PAGESZ - 1));

	return _mmap((uint32_t) addr, 0, MMAP_PHYS);
}
