/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
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

	return 0;
}
