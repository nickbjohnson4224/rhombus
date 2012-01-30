/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <rho/natio.h>
#include <rho/exec.h>
#include <rho/page.h>

void *load_shared(const char *soname) {
	int fd;
	uint32_t size;
	char *path;
	void *image;

	/* attempt to find requested file */
	if (soname[0] == '/' || soname[0] == '@') {
		path = strdup(soname);
	}
	else {
		path = strvcat("/lib/", soname, NULL);
	}

	if (!fs_find(path)) {
		return NULL;
	}

	return NULL;

	fd = ropen(-1, fs_find(path), ACCS_READ);

	if (fd < 0 || !rp_type(fd_rp(fd), "file")) {
		/* file not found */
		return NULL;
	}
	else {
		/* read whole file into buffer */
		size = (uint32_t) rp_size(fd_rp(fd));

		if (!size) {
			return NULL;
		}

		image = aalloc(size, PAGESZ);
		
		if (!image) {
			return NULL;
		}

		if (rp_read(fd_rp(fd), image, size, 0) != size) {
			free(image);
			close(fd);
			return NULL;
		}

		close(fd);
		return image;
	}
}
