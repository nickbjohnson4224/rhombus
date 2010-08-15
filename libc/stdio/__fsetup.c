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
#include <errno.h>

/****************************************************************************
 * __fsetup
 *
 * Set up stream extensions for a file with default values. Returns pointer
 * to set up stream on success, NULL on failure.
 */

FILE *__fsetup(FILE *stream) {
	
	if (!stream) {
		errno = ERANGE;
		return NULL;
	}

	stream->ext = malloc(sizeof(struct _file_ext));

	if (!stream->ext) {
		errno = ENOMEM;
		return NULL;
	}

	stream->ext->mutex    = false;
	stream->ext->position = 0;
	stream->ext->size     = -1;
	stream->ext->buffer   = NULL;
	stream->ext->buffsize = 0;
	stream->ext->buffpos  = 0;
	stream->ext->revbuf   = EOF;
	stream->ext->flags    = FILE_NBF | FILE_READ | FILE_WRITE;

	return stream;
}
