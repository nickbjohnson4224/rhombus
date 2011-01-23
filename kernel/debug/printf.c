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

#include <debug.h>
#include <stdarg.h>
#include <stdint.h>

/****************************************************************************
 * debug_printf
 *
 * Prints to debugging output using a similar interface to printf().
 */

void debug_printf(const char *format, ...) {
	size_t i, fbt;
	char buffer[13];
	char fmtbuffer[100];
	const char *str;
	va_list ap;
	char c;

	va_start(ap, format);

	fbt = 0;

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			fmtbuffer[fbt] = '\0';
			debug_string(fmtbuffer);
			fbt = 0;

			switch (format[i+1]) {
			case 'x':
			case 'X':
				__itoa(buffer, va_arg(ap, int), 16);
				debug_string(buffer);
				break;
			case 'd':
			case 'i':
				__itoa(buffer, va_arg(ap, int), 10);
				debug_string(buffer);
				break;
			case 's':
				str = va_arg(ap, const char*);
				if (str) {
					debug_string(str);
				}
				else {
					debug_string("(null)");
				}
				break;
			case 'c':
				c = va_arg(ap, int);
				debug_char(c);
				break;
			case '%':
				c = '%';
				debug_char('%');
				break;
			}
			i++;
		}
		else {
			fmtbuffer[fbt++] = format[i];
		}
	}
	
	fmtbuffer[fbt] = '\0';
	debug_string(fmtbuffer);

	va_end(ap);
}
