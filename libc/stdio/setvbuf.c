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

#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * setvbuf
 *
 * Set the buffering properties and buffer size of a stream.
 */

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	
	fflush(stream);

	if (!stream->ext) {
		__fsetup(stream);
	}

	switch (mode) {
	case _IONBF:
		if (stream->ext->buffer) {
			free(stream->ext->buffer);
			stream->ext->buffer = NULL;
		}
		stream->ext->buffsize = 0;
		break;
	case _IOLBF:
	case _IOFBF:
		if (stream->ext->buffer) {
			free(stream->ext->buffer);
		}
		if (buf) {
			stream->ext->buffer = (uint8_t*) buf;
		}
		else {
			stream->ext->buffer = malloc(size);
		}
		stream->ext->buffsize = size;
		stream->ext->buffpos = 0;
		break;
	default:
		return -1;
	}

	stream->ext->flags &= ~(FILE_FBF | FILE_LBF | FILE_NBF);

	switch (mode) {
	case _IONBF:
		stream->ext->flags |= FILE_NBF;
		break;
	case _IOLBF:
		stream->ext->flags |= FILE_LBF;
		break;
	case _IOFBF:
		stream->ext->flags |= FILE_FBF;
		break;
	}

	return 0;
}
