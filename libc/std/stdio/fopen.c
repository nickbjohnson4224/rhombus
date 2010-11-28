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
	char *path1, *path_simp;
	FILE *stream;

	/* check mode */
	if (mode[0] != 'a' && mode[0] != 'w' && mode[0] != 'r') {
		errno = EINVAL;
		return NULL;
	}

	/* attempt to find the file */
	path_simp = path_simplify(path);
	if (!path_simp) {
		return NULL;
	}
	fd = fs_find(0, path_simp);

	/* check if the object is a directory or null */
	if (fd && fs_type(fd) == FOBJ_DIR) {
		errno = EISDIR;
		free(path_simp);
		return NULL;
	}

	if (!fd) {

		/* create the file */
		if (mode[0] == 'w' || mode[0] == 'a') {

			/* find requested parent directory */
			path1 = path_parent(path_simp);
			if (!path1) {
				errno = ENOMEM;
				free(path_simp);
				return NULL;
			}
			fd = fs_find(0, path1);
			free(path1);

			if (!fd) {
				errno = ENOENT;
				free(path_simp);
				return NULL;
			}
			else {
				/* check permissions of directory */
				if ((fs_perm(fd, gettuser()) & ACL_WRITE) == 0) {
					errno = EACCES;
					free(path_simp);
					return NULL;
				}
			}

			/* create file in parent directory */
			path1 = path_name(path_simp);
			if (!path1) {
				errno = ENOMEM;
				free(path_simp);
				return NULL;
			}
			fd = fs_cons(fd, path1, FOBJ_FILE);
			free(path1);
			free(path_simp);

			if (!fd) {
				errno = EACCES;
				return NULL;
			}
		}
		else {
			free(path_simp);
			errno = ENOENT;
			return NULL;
		}
	}

	free(path_simp);

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

	/* open a stream on the file */
	stream = malloc(sizeof(FILE));

	if (!stream) {
		errno = ENOMEM;
		return NULL;
	}

	stream->fd       = fd;
	stream->mutex    = false;
	stream->position = 0;
	stream->size     = fs_size(fd);
	stream->buffer   = NULL;
	stream->buffsize = 0;
	stream->buffpos  = 0;
	stream->revbuf   = EOF;
	stream->flags    = FILE_NBF | FILE_READ;

	if (mode[0] == 'w' || mode[0] == 'a' || mode[1] == '+') {
		stream->flags |= FILE_WRITE;
	}

	/* position the stream properly */
	if (mode[0] == 'a' && mode[1] != '+') {
		fseek(stream, 0, SEEK_END);
	}
	else {
		fseek(stream, 0, SEEK_SET);
	}

	return stream;
}
