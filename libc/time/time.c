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

#include <time.h>
#include <stdio.h>
#include <natio.h>
#include <stdint.h>
#include <stdlib.h>

/****************************************************************************
 * _time_dev
 *
 * File representing the current clock source.
 */

static FILE *_time_dev = NULL;

/****************************************************************************
 * time
 *
 * Returns the time as the number of seconds since Jan. 1, 1970, 00:00:00 
 * UTC. If <timer> is non-null, the return value is also stored in the memory
 * pointed to by <timer>. Returns (time_t) -1 on error.
 */

time_t time(time_t *timer) {
	char buffer[21];
	size_t count;
	time_t value;

	if (!_time_dev) {
		_time_dev = fopen("/dev/time", "r");

		if (!_time_dev) {
			return (time_t) -1;
		}
	}

	count = fread(buffer, sizeof(char), 21, _time_dev);

	if (count == 0) {
		return (time_t) -1;
	}

	buffer[20] = '\0';
	sscanf(buffer, "%d", &value);

	return value;
}
