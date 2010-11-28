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

#include <stdlib.h>
#include <driver.h>
#include <stdio.h>
#include <natio.h>
#include <errno.h>

/****************************************************************************
 * fopen
 *
 * The fopen() function opens the file whose name is the string pointed to
 * by path and associates a stream with it. Returns the newly opened stream
 * on success, and NULL on failure.
 */

FILE *fopen(const char *path, const char *mode) {
	uint64_t fd;
	uint8_t perm;
	char *path1;

	/* attempt to find the file */
	fd = fs_find(0, path);

	/* check if the object is a directory or null */
	if (fd && fs_type(fd) == FOBJ_DIR) {
		errno = EISDIR;
		return NULL;
	}

	if (!fd) {

		/* create the file */
		if (mode[0] == 'w' || mode[0] == 'a') {

			/* find requested parent directory */
			path1 = path_parent(path);
			fd = fs_find(0, path1);
			free(path1);

			if (!fd) {
				errno = ENOENT;
				return NULL;
			}
			else {
				/* check permissions of directory */
				if ((fs_perm(fd, gettuser()) & ACL_WRITE) == 0) {
					errno = EACCES;
					return NULL;
				}
			}

			/* create file in parent directory */
			path1 = path_name(path);
			fd = fs_cons(fd, path1, FOBJ_FILE);
			free(path1);

			if (!fd) {
				errno = EACCES;
				return NULL;
			}
		}
		else {
			errno = ENOENT;
			return NULL;
		}
	}

	perm = fs_perm(fd, gettuser());

	/* check read permissions */
	if ((perm & ACL_READ) == 0) {
		errno = EACCES;
		return NULL;
	}

	/* check write permissions */
	if (mode[0] == 'w' || mode[0] == 'a' || mode[1] == '+') {
		if ((perm & ACL_WRITE) == 0) {
			errno = EACCES;
			return NULL;
		}
	}

	/* reset the file contents */
	if (mode[0] == 'w') {
		reset(fd);
	}

	return fdopen(fd, mode);
}
