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

#include <debug.h>

/****************************************************************************
 * __itoa
 *
 * Convert the unsigned integer <n> to a string in base <base> and store in
 * <buffer>. Returns 0 on success, nonzero on error.
 */

void __itoa(char *buffer, size_t n, size_t base) {
	const char *d;
	char temp;
	size_t i, size;

	d = "0123456789ABCDEF";

	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (base > 16) {
		buffer[0] = '\0';
		return;
	}

	for (i = 0; n; i++) {
		buffer[i] = d[n % base];
		n /= base;
	}

	buffer[i] = '\0';
	size = i;

	for (i = 0; i < (size / 2); i++) {
		temp = buffer[size - i - 1];
		buffer[size - i - 1] = buffer[i];
		buffer[i] = temp;
	}

	return;
}
