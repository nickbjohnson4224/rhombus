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
#include <stdarg.h>
#include <string.h>

/****************************************************************************
 * vsprintf
 *
 * XXX - doc
 */

int vsprintf(char *str, const char *format, va_list ap) {
	size_t i;
	char m[2], buffer[13];

	strcpy(str, "");
	m[1] = '\0';

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			switch (format[i+1]) {
			case 'x':
				__utoa(buffer, va_arg(ap, int), 16, false);
				strcat(str, buffer);
				break;
			case 'X':
				__utoa(buffer, va_arg(ap, int), 16, true);
				strcat(str, buffer);
				break;
			case 'u':
				__utoa(buffer, va_arg(ap, int), 16, false);
				strcat(str, buffer);
				break;
			case 'd':
			case 'i':
				__itoa(buffer, va_arg(ap, int), 10, false);
				strcat(str, buffer);
				break;
			case 'o':
			case 'O':
				__utoa(buffer, va_arg(ap, int), 8, false);
				strcat(str, buffer);
				break;
			case 's':
				strcat(str, va_arg(ap, const char*));
				break;
			case 'c':
				m[0] = va_arg(ap, int);
				strcat(str, m);
				break;
			case '%':
				m[0] = '%';
				strcat(str, m);
				break;
			}
			i++;
		}
		else {
			m[0] = format[i];
			strcat(str, m);
		}
	}

	return i;
}
