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

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <rho/natio.h>
#include <rho/mutex.h>

static int __bufwrite(FILE *stream);

static int __nobufwrite(FILE *stream, void *data, size_t size);

/****************************************************************************
 * fwrite
 *
 * Writes <nmemb> * <size> bytes of data from <stream> from the buffer <ptr>.
 * Returns the number of bytes / <size> successfully written; on error or 
 * EOF, a lessened or zero value is returned.
 */

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	const uint8_t *data = ptr;
	size_t i, ret, count;
	
	if (!stream) {
		return 0;
	}

	if (!(size * nmemb)) {
		return 0;
	}

	mutex_spin(&stream->mutex);

	// no buffering
	if (stream->flags & FILE_NBF) {

		ret = rp_write(fd_rp(stream->fd), fd_getkey(stream->fd, AC_WRITE),
			(void*) ptr, size * nmemb, stream->position);
		stream->position += ret;

		if (size == 0) {
			size = 1;
		}

		mutex_free(&stream->mutex);

		return (ret / size);
	}

	// line buffering
	if (stream->flags & FILE_LBF) {
		
		for (i = 0; i < size * nmemb; i++) {
			stream->buffer[stream->buffpos++] = data[i];

			if ((data[i] == '\n') || (stream->buffpos >= stream->buffsize)) {
				if (__bufwrite(stream)) {
					mutex_free(&stream->mutex);
					return 0;
				}
			}
		}
		
		mutex_free(&stream->mutex);

		return nmemb;
	}

	// full buffering
	count = size * nmemb;
	if (stream->buffpos + count >= stream->buffsize) {
		if (__bufwrite(stream)) {
			mutex_free(&stream->mutex);
			return 0;
		}
		if (__nobufwrite(stream, (void*) ptr, count)) {
			mutex_free(&stream->mutex);
			return 0;
		}
	}
	else {
		memcpy(&stream->buffer[stream->buffpos], (void*) ptr, count);
		stream->buffpos += count;
	}
	
	mutex_free(&stream->mutex);

	return nmemb;
}

static int __bufwrite(FILE *stream) {
	size_t count;
	size_t pos;
	
	for (pos = 0; pos < stream->buffpos;) {

		count = rp_write(fd_rp(stream->fd), fd_getkey(stream->fd, AC_WRITE), 
			stream->buffer, stream->buffpos - pos, stream->position);

		if (count == 0) {
			return -1;
		}

		stream->position += count;
		pos += count;
	}

	stream->buffpos = 0;

	return 0;
}

static int __nobufwrite(FILE *stream, void *data, size_t size) {
	size_t count;
	size_t pos;
	
	for (pos = 0; pos < size;) {

		count = rp_write(fd_rp(stream->fd), fd_getkey(stream->fd, AC_WRITE),
			data, size - pos, stream->position);

		if (count == 0) {
			return -1;
		}

		stream->position += count;
		pos += count;
	}

	return 0;
}
