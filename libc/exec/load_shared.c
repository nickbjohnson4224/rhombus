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

#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <exec.h>
#include <page.h>

/*****************************************************************************
 * load_shared
 *
 * Load a shared library image in read-only shared memory from the shared
 * object daemon.
 */

void *load_shared(const char *soname) {
	uint64_t fd;
	char *path;

	path = strvcat("/sys/lib/", soname, NULL);
	fd = fs_find(0, path);
	free(path);

	if (!fd) {
		return NULL;
	}
	else {
		return mmap(fd, fs_size(fd), PROT_READ | PROT_EXEC, 0);
	}
}
