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
#include <errno.h>
#include <string.h>

/****************************************************************************
 * __utoa
 *
 * Convert the unsigned integer <n> to a string in base <base> and store in
 * <buffer>. Letters (for bases > 10) are lowercase if <ucase> is false, and
 * uppercase if <ucase> is true. Returns 0 on success, nonzero on error. 
 * Used internally by vsnprintf().
 */

int __utoa(char *buffer, unsigned int n, int b, bool ucase) {
	const char *d;
	char temp;
	size_t i, size;

	d = (ucase) ? "0123456789ABCDEF" : "0123456789abcdef";

	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 0;
	}

	if (b > 16) {
		buffer[0] = '\0';
		errno = ERANGE;
		return -1;
	}

	for (i = 0; n; i++) {
		buffer[i] = d[n % b];
		n /= b;
	}

	buffer[i] = '\0';
	size = i;

	for (i = 0; i < (size / 2); i++) {
		temp = buffer[size - i - 1];
		buffer[size - i - 1] = buffer[i];
		buffer[i] = temp;
	}

	return 0;
}
