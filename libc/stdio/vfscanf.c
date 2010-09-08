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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

/****************************************************************************
 * _scanword
 */

static int _scanword(char *buffer, FILE *stream, size_t length, int c) {
	size_t i;

	for (i = 0; i < length - 1; i++) {
		buffer[i] = (char) c;
		c = fgetc(stream);

		if (isspace((char) c) || c == EOF) {
			break;
		}
	}

	buffer[i+1] = '\0';

	return 0;
}

/****************************************************************************
 * vfscanf
 *
 * XXX - doc
 */

int vfscanf(FILE *stream, const char *format, va_list ap) {
	char buffer[20], *s;
	int c, *d;

	while (*format) {

		while (isspace(c = fgetc(stream))) {
			if (c == EOF) {
				return -1;
			}
		}

		while (*format != '%') {
			format++;
		}

		if (*format == '%') {
			format++;
			switch (*format) {
			case 's':
				if (_scanword(va_arg(ap, char*), stream, -1, c)) {
					return -1;
				}
				break;
			case 'd': 
			case 'i':
				d = va_arg(ap, int*);
				
				if (_scanword(buffer, stream, 20, c)) {
					return -1;
				}

				*d = atoi(buffer);
				break;
			case 'c':
				s = va_arg(ap, char*);
				*s = c;
				break;
			}
		}

		format++;
	}

	return 0;
}
