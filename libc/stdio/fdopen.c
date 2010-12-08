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
#include <stdio.h>
#include <natio.h>
#include <errno.h>

/****************************************************************************
 * fdopen
 *
 * Create a stream from the file descriptor <fd> with mode <mode>. Returns
 * a pointer to the new stream on success, NULL on failure.
 */

#define MODE_WRTE 0x01
#define MODE_RSET 0x02
#define MODE_CONS 0x04

FILE *fdopen(uint64_t fd, const char *mode) {
	FILE *stream;
	
	if (!fd) {
		return NULL;
	}

	stream = calloc(sizeof(FILE), 1);

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
	stream->flags    = FILE_NBF | FILE_READ | FILE_WRITE;

	return stream;
}
