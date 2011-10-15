/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
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
	FILE *stream;
	int status;

	// check mode
	if (mode[0] != 'a' && mode[0] != 'w' && mode[0] != 'r') {
		errno = EINVAL;
		return NULL;
	}

	// attempt to find the file
	fd = fs_find(path);

	// check if the object is not a file
	if (fd && !rp_type(fd, "file")) {
		errno = EISDIR;
		return NULL;
	}

	if (!fd) {

		// create the file
		if (mode[0] == 'w' || mode[0] == 'a') {
			fd = fs_cons(path, "file");
			if (!fd) {
				return NULL;
			}
		}
		else {
			errno = ENOENT;
			return NULL;
		}
	}

	status = 0;

	// check read permissions
	if (mode[0] == 'r' || mode[1] == '+') {
		status |= STAT_READER;
	}

	// check write permissions
	if (mode[0] == 'w' || mode[0] == 'a' || mode[1] == '+') {
		status |= STAT_WRITER;
	}

	// open file for real
	if (rp_open(RP_CURRENT_PROC, fd, status)) {
		return NULL;
	}

	// reset (erase) the file contents
	if (mode[0] == 'w') {
		reset(fd);
	}

	// open a stream on the file
	stream = malloc(sizeof(FILE));

	if (!stream) {
		errno = ENOMEM;
		return NULL;
	}

	// set up stream structure
	stream->fd       = fd;
	stream->mutex    = false;
	stream->position = 0;
	stream->size     = rp_size(fd);
	stream->buffer   = NULL;
	stream->buffsize = 0;
	stream->buffpos  = 0;
	stream->revbuf   = EOF;
	stream->flags    = FILE_NBF | FILE_READ;

	if (mode[0] == 'w' || mode[0] == 'a' || mode[1] == '+') {
		stream->flags |= FILE_WRITE;
	}

	// position the stream properly
	if (mode[0] == 'a' && mode[1] != '+') {
		fseek(stream, 0, SEEK_END);
	}
	else {
		fseek(stream, 0, SEEK_SET);
	}

	return stream;
}
