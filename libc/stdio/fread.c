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
#include <natio.h>

/****************************************************************************
 * fread
 *
 * Reads <nmemb> * <size> bytes of data from <stream> into the buffer <ptr>.
 * Returns the number of bytes / <size> successfully read; on error or EOF,
 * a lessened or zero value is returned.
 */

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t ret, rev_count;
	char *cptr;

	if (!stream) {
		return 0;
	}

	nmemb *= size;

	if ((stream->revbuf != EOF) && (nmemb != 0)) {
		cptr = ptr;
		cptr[0] = stream->revbuf;
		ptr = &cptr[1];
		stream->revbuf = EOF;
		rev_count = 1;
	}
	else {
		rev_count = 0;
	}

	ret = read(stream->fd, ptr, nmemb - rev_count, stream->position) + rev_count;
	stream->position += ret;

	if (ret != nmemb) {
		stream->flags |= FILE_EOF;
	}

	if (size == 0) {
		size = 1;
	}

	return (ret / size);
}
