/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
 * load_exec
 *
 * Load an executable image into memory. This function does searches through
 * PATH if <name> is a relative path. Currently, only single-directory PATHs
 * are supported. Returns a page-aligned pointer to the executable image on
 * success, NULL on failure.
 */

void *load_exec(const char *name) {
	uint64_t fd;
	uint64_t size;
	char *path;
	void *image;

	/* attempt to find requested file */
	if (name[0] == '/' || name[0] == '@') {
		path = strdup(name);
	}
	else {
		path = strvcat(getenv("PATH"), "/", name, NULL);
	}
	fd = fs_find(path);

	if (!fd) {
		/* file not found */
		return NULL;
	}
	else {
		/* read whole file into buffer */
		size = fs_size(path);

		if (!size) {
			return NULL;
		}

		image = aalloc(size, PAGESZ);
		
		if (!image) {
			return NULL;
		}

		if (read(fd, image, size, 0) != size) {
			free(image);
			return NULL;
		}

		return image;
	}
}
