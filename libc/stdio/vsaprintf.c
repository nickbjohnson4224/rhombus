/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/****************************************************************************
 * vsaprintf
 *
 * Prints a formatted string to an allocated buffer, then returns the
 * buffer. Returns null on error.
 */

char *vsaprintf(const char *format, va_list ap) {
	char *string0;
	char *string1;
	int i;
	char m[2];
	const char *format_tmp;

	string0 = strdup("");
	m[1] = '\0';

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			format_tmp = &format[i];
			string1 = string0;
			string0 = strvcat(string1, __format(&format_tmp, &ap), NULL);
			free(string1);
			i = (uintptr_t) format_tmp - (uintptr_t) format;
		}
		else {
			m[0] = format[i];
			string1 = string0;
			string0 = strvcat(string1, m);
			free(string1);
		}
	}

	return string0;
}
