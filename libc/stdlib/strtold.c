/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

static long double __scan_float_uint(const char **_str, int flags) {
	long double value;

	value = 0;
	while (isdigit(**_str)) {
		value *= 10;
		value += (**_str - '0');
		(*_str)++;
	}

	return value;
}

static long double __scan_float_int(const char **_str, int flags) {

	if (**_str == '-') {
		(*_str)++;
		return -__scan_float_uint(_str, flags);
	}
	else if (**_str == '+') {
		(*_str)++;
		return __scan_float_uint(_str, flags);
	}
	else {
		return __scan_float_uint(_str, flags);
	}
}

static long double __scan_float(const char **_str, int flags) {
	long double i_part;
	long double f_part;
	long double e_part;
	long double f_exp;

	i_part = __scan_float_int(_str, flags);

	if (**_str == '.') {
		(*_str)++;
		f_part = __scan_float_uint(_str, flags);
	}
	else {
		f_part = 0.0;
	}

	if (**_str == 'e' || **_str == 'E') {
		(*_str)++;
		e_part = __scan_float_int(_str, flags);
	}
	else {
		e_part = 0.0;
	}

	f_exp = ceill(log10l(f_part));
	if (f_exp < 1) f_exp = 1.0;

	return (i_part + f_part / powl(10, f_exp)) * powl(10, e_part);
}

/****************************************************************************
 * strtold
 *
 * Convert a string to a floating point number.
 */

long double strtold(const char *nptr, char **endptr) {
	long double value;

	value = __scan_float(&nptr, 0);

	if (endptr) {
		*endptr = (char*) nptr;
	}

	return value;
} 
