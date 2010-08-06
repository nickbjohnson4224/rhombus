/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <dict.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>

/****************************************************************************
 * dalloc
 *
 * Returns a pointer to a readable and writable region of memory of at least 
 * size <size>, which is persisted when a process executes and is garbage
 * collected based on the state of the dictionary. This function is not
 * thread safe outside of thread safe dictionary functions.
 */

void *dalloc(size_t size) {
	void *ptr;

	ptr = (void*) dict_info->brk;
	dict_info->brk += size;

	mmap(ptr, size + PAGESZ, MMAP_READ | MMAP_WRITE);

	memclr(ptr, size);
	return ptr;
}
