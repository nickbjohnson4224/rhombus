/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

/****************************************************************************
 * fclose - close a stream
 *
 * The fclose() function flushes the stream pointed to by fp (writing any
 * buffered output data using fflush()), and closes the underlying file
 * descriptor.
 *
 * Upon successful completion 0 is returned. Otherwise, EOF is returned and
 * errno is set to indicate the error. In either case and further access
 * (including another call to fclose()) to the stream results in undefined
 * behavior.
 */

int fclose(FILE *fp) {

	/* flush any buffers */
	fflush(fp);

	/* free any buffers */
	if (fp->buffer) {
		free(fp->buffer);
	}

	/* close file descriptor */
	close(fp->filedes);

	/* free file structure */
	free(fp);

	return 0;
}
