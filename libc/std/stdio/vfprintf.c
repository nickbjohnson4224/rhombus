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
#include <string.h>
#include <stdarg.h>

/****************************************************************************
 * vfprintf
 *
 * XXX - doc
 */

int vfprintf(FILE *stream, const char *format, va_list ap) {
	size_t i, fbt;
	char buffer[100];
	char *fmtbuffer;
	const char *str;
	char c;

	if (!stream) {
		return -1;
	}

	fmtbuffer = malloc(sizeof(char) * strlen(format));
	fbt = 0;

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			fwrite(fmtbuffer, sizeof(char), fbt, stream);
			fbt = 0;

			switch (format[i+1]) {
			case 'x':
				__utoa(buffer, va_arg(ap, int), 16, false);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'X':
				__utoa(buffer, va_arg(ap, int), 16, true);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'u':
				__utoa(buffer, va_arg(ap, int), 10, false);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'd':
			case 'i':
				__itoa(buffer, va_arg(ap, int), 10, false);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'o':
				__utoa(buffer, va_arg(ap, int), 8, false);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'O':
				__utoa(buffer, va_arg(ap, int), 8, false);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 's':
				str = va_arg(ap, const char*);
				if (str) {
					fwrite(str, strlen(str), sizeof(char), stream);
				}
				else {
					fprintf(stream, "(null)");
				}
				break;
			case 'c':
				c = va_arg(ap, int);
				fwrite(&c, 1, sizeof(char), stream);
				break;
			case 'f':
				__ftoa(buffer, va_arg(ap, double), 16);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'e':
				__etoa(buffer, va_arg(ap, double), 16);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case '%':
				c = '%';
				fwrite(&c, 1, sizeof(char), stream);
				break;
			}
			i++;
		}
		else {
			fmtbuffer[fbt++] = format[i];
		}
	}
	
	fwrite(fmtbuffer, fbt, sizeof(char), stream);
	free(fmtbuffer);

	return 0;
}

