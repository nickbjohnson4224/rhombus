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
#include <ctype.h>
#include <math.h>

/****************************************************************************
 * strtod
 *
 * Convert a string to a floating point number.
 */

double strtod(const char *nptr, char **endptr) {
	double sum;
	int i;

	for (sum = 0, i = 0; nptr[i] && isdigit(nptr[i]); i++) {
		sum *= 10;
		sum += __digit(nptr[i], 10);
	}

	if (nptr[i] == '.') {
		for (; nptr[i] && isdigit(nptr[i]); i++) {
//			sum += __digit(nptr[i], 10) * pow(10, -j);
		}
	}

	if (endptr) {
		*endptr = (char*) &nptr[i];
	}

	return sum;
} 
