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

#include <stdlib.h>
#include <ctype.h>

/****************************************************************************
 * strtoul
 *
 * Convert the contents of a string containing a <base>-ary number to an 
 * unsigned long integer. If endptr is non-NULL, a pointer to the character
 * after the last character parsed is stored in it.
 */

uint32_t strtoul(const char *nptr, char **endptr, int base) {
	int i;
	uint32_t sum;

	for (sum = 0, i = 0; nptr[i] && isdigit(nptr[i]); i++) {
		sum *= base;
		sum += __digit(nptr[i], base);
	}

	if (endptr) {
		*endptr = (char*) &nptr[i];
	}

	return sum;
}
