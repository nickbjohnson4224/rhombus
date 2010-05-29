/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>

#include <flux/io.h>

/****************************************************************************
 * fflush
 *
 * Forces a write of all userspace buffered data for the given output stream
 * via the stream's underlying write function.
 */

int fflush(FILE *stream) {

	if (stream->buffer && stream->buffpos) {
		write(stream->filedes, stream->buffer, stream->buffpos, stream->position);
		stream->position += stream->buffpos;
		stream->buffpos = 0;
	}

	return 0;
}
