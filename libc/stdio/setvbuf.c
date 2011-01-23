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

#include <stdlib.h>
#include <stdio.h>
#include <mutex.h>
#include <errno.h>

/****************************************************************************
 * setvbuf
 *
 * Set the buffering properties and buffer size of a stream.
 */

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	
	fflush(stream);

	mutex_spin(&stream->mutex);

	switch (mode) {
	case _IONBF:
		if (stream->buffer) {
			free(stream->buffer);
			stream->buffer = NULL;
		}
		stream->buffsize = 0;
		break;
	case _IOLBF:
	case _IOFBF:
		if (stream->buffer) {
			free(stream->buffer);
		}
		if (buf) {
			stream->buffer = (uint8_t*) buf;
		}
		else {
			stream->buffer = malloc(size);
		}
		stream->buffsize = size;
		stream->buffpos = 0;
		break;
	default:
		errno = EINVAL;
		mutex_free(&stream->mutex);
		return -1;
	}

	stream->flags &= ~(FILE_FBF | FILE_LBF | FILE_NBF);

	switch (mode) {
	case _IONBF:
		stream->flags |= FILE_NBF;
		break;
	case _IOLBF:
		stream->flags |= FILE_LBF;
		break;
	case _IOFBF:
		stream->flags |= FILE_FBF;
		break;
	}

	mutex_free(&stream->mutex);

	return 0;
}
