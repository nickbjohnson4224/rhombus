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

#include <stdio.h>
#include <natio.h>
#include <mutex.h>

/****************************************************************************
 * fflush
 *
 * Forces a write of all userspace buffered data for the given output stream
 * via the stream's underlying write function.
 */

int fflush(FILE *stream) {

	if (!stream) {
		return -1;
	}

	mutex_spin(&stream->mutex);

	if (stream->buffer && stream->buffpos) {
		rp_write(fd_rp(stream->fd), stream->buffer, stream->buffpos, stream->position);
		stream->position += stream->buffpos;
		stream->buffpos = 0;
	}

	mutex_free(&stream->mutex);

	return 0;
}
