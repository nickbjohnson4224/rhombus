/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/exec.h>
#include <flux/mmap.h>

static uint8_t *bootstrap = (void*) ESPACE;

int exec(uint8_t *image, size_t size) {
	
	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	arch_memcpy(bootstrap, image, size);

	return _exec((uintptr_t) bootstrap);
}
