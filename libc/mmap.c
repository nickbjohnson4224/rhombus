#include <mmap.h>
#include "libc.h"

int mmap(void *addr, size_t length, int prot) {
	return __mmap((uint32_t) addr, length, prot);
}

int umap(void *addr, size_t length) {
	return __umap((uint32_t) addr, length);
}

int emap(void *addr, uint32_t frame, int prot) {
	return __emap((uint32_t) addr, frame, prot);
}
