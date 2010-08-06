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

#include <dict.h>
#include <string.h>
#include <ctype.h>

static char tohex(int n) {
	if (n > 9) {
		return (char) ('A' + n - 10);
	}
	else {
		return (char) ('0' + n);
	}
}

static int fromhex(char c) {
	if (isdigit(c)) {
		return (int) (c - '0');
	}
	else {
		return (int) (c - 'A');
	}
}

char *deflate(const void *data, size_t size, char *arcbuffer) {
	size_t i, j;
	const uint8_t *bytes;

	if (!data) return NULL;

	bytes = data;

	arcbuffer[0] = 'e'; // little endian

	for (i = 0, j = 1; i < size; i++) {
		arcbuffer[j++] = tohex(bytes[i] >> 4);
		arcbuffer[j++] = tohex(bytes[i] & 15);
	}

	arcbuffer[j] = '\0';

	return arcbuffer;
}

size_t inflate(void *datbuffer, size_t size, const char *arc) {
	size_t i, j;
	uint8_t *bytes;

	if (!arc) return 0;

	bytes = datbuffer;
	
	for (i = 0, j = 1; i < size && arc[j]; i++) {
		bytes[i] = (fromhex(arc[j++]) << 4);
		bytes[i] |= fromhex(arc[j++]);
	}

	return i;
}
