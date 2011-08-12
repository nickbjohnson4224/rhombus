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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/****************************************************************************
 * strtoul
 *
 * Convert the contents of a string containing a <base>-ary number to an 
 * unsigned long integer. If endptr is non-NULL, a pointer to the character
 * after the last character parsed is stored in it.
 */

unsigned long int strtoul(const char *nptr, char **endptr, int base) {
	size_t i;
	uint32_t sum;
	bool neg;

	i = 0;
	sum = 0;

	while (isspace(nptr[i])) {
		i++;
	}

	if (nptr[i] == '+') {
		neg = false;
		i++;
	}
	else if (nptr[i] == '-') {
		neg = true;
		i++;
	}

	if (base == 16 && !strncmp(nptr, "0x", 2)) {
		i += 2;
	}
	if (base == 0) {
		if (nptr[i] == '0') {
			i++;
			if (nptr[i] == 'x') {
				i++;
				base = 16;
			}
			else if (__isbdigit(nptr[i], 8)) {
				base = 8;
			}
		}
		else if (__isbdigit(nptr[i], 10)) {
			base = 10;
		}
	}

	if (base == 0) {
		if (endptr) {
			*endptr = (char*) &nptr[i];
		}

		return 0;
	}

	while (nptr[i] && __isbdigit(nptr[i], base)) {
		sum *= base;
		sum += __digit(nptr[i], base);
		i++;
	}

	if (endptr) {
		*endptr = (char*) &nptr[i];
	}

	return (neg) ? -sum : sum;
}
