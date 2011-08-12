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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#define TYPE_INT	0x01
#define TYPE_UINT	0x02
#define TYPE_FLOAT	0x03
#define TYPE_STR	0x04
#define TYPE_CHAR	0x05

#define FLAGS_DEC	0x01
#define FLAGS_HEX	0x02
#define FLAGS_OCT	0x04
#define FLAGS_EXP	0x08
#define FLAGS_UPPER	0x10
#define FLAGS_IGN	0x20
#define FLAGS_LONG	0x40
#define FLAGS_LLONG	0x80

static unsigned int __scan_uint(FILE *stream, int width, int flags) {
	int base;
	char c;
	unsigned int value;

	/* decide base */
	if (flags & FLAGS_DEC) base = 10;
	else if (flags & FLAGS_HEX) base = 16;
	else if (flags & FLAGS_OCT) base = 8;
	else {
		c = getc(stream);
		if (c == '0') {
			width--;

			c = getc(stream);
			if (c == 'x' || c == 'X') {
				width--;
				base = 16;
			}
			else {
				base = 8;
			}
		}
		else {
			ungetc(c, stream);
			base = 10;
		}
	}

	value = 0;
	while (__isbdigit((c = getc(stream)), base)) {
		value *= base;
		value += __digit(c, base);
		width--;

		if (width == 0) {
			return value;
		}
	}

	ungetc(c, stream);
	return value;
}

static int __scan_int(FILE *stream, int width, int flags) {
	char c;

	c = getc(stream);
	if (c == '-') {
		return -__scan_uint(stream, width - 1, flags);
	}
	else {
		ungetc(c, stream);
		return __scan_uint(stream, width - 1, flags);
	}
}

static char *__scan_str(FILE *stream, int width, int flags) {
	int i;
	char buffer[100];

	for (i = 0; i < width; i++) {
		buffer[i] = getc(stream);
		if (isspace(buffer[i])) {
			ungetc(buffer[i], stream);
			buffer[i] = '\0';
			break;
		}
	}

	return strdup(buffer);
}

static char __scan_char(FILE *stream, int flags) {
	return getc(stream);
}

static long double __scan_float_uint(FILE *stream, int flags) {
	long double value;
	char c;

	value = 0;
	while (isdigit((c = getc(stream)))) {
		value *= 10;
		value += (c - '0');
	}

	ungetc(c, stream);
	return value;
}

static long double __scan_float_int(FILE *stream, int flags) {
	char c;

	c = getc(stream);
	if (c == '-') {
		return -__scan_float_uint(stream, flags);
	}
	else if (c == '+') {
		return __scan_float_uint(stream, flags);
	}
	else {
		ungetc(c, stream);
		return __scan_float_uint(stream, flags);
	}
}

static long double __scan_float(FILE *stream, int flags) {
	long double i_part;
	long double f_part;
	long double e_part;
	char c;

	i_part = __scan_float_int(stream, flags);

	c = getc(stream);
	if (c == '.') {
		f_part = __scan_float_uint(stream, flags);
	}
	else {
		ungetc(c, stream);
		f_part = 0.0;
	}

	c = getc(stream);
	if (c == 'e' || c == 'E') {
		e_part = __scan_float_int(stream, flags);
	}
	else {
		ungetc(c, stream);
		e_part = 0.0;
	}

	return (i_part + f_part / powl(10, ceill(log10l(f_part)))) * powl(10, e_part);
}

static void __scan_space(FILE *stream) {
	char c;

	while (isspace(c = getc(stream)));
	ungetc(c, stream);
}

int vfscanf(FILE *stream, const char *format, va_list ap) {
	int flags;
	int width;
	int count;
	int type;

	int *i;
	unsigned int *u;
	float *f;
	double *lf;
	long double *Lf;
	char **s;
	char *c;

	count = 0;
	while (*format) {

		if (*format == ' ') {
			__scan_space(stream);
		}
		else if (*format == '%') {
			flags = 0;
			format++;

			/* check for ignore flag */
			if (*format == '*') {
				flags |= FLAGS_IGN;
				format++;
			}

			/* check for width */
			if (isdigit(*format)) {
				width = atoi(format);
				while (isdigit(*format)) format++;
			}
			else {
				width = -1;
			}

			/* check for length modifier */
			if (*format == 'l') {
				flags |= FLAGS_LONG;
				format++;
			}
			else if (*format == 'L') {
				flags |= FLAGS_LLONG;
				format++;
			}

			/* check type */
			switch (*format) {
			case 'd':
				flags |= FLAGS_DEC;
				type = TYPE_INT;
				break;
			case 'i':
				type = TYPE_INT;
				break;
			case 'X': flags |= FLAGS_UPPER;
			case 'x':
				flags |= FLAGS_HEX;
				type = TYPE_UINT;
				break;
			case 'o':
				flags |= FLAGS_OCT;
				type = TYPE_UINT;
				break;
			case 's':
				type = TYPE_STR;
				break;
			case 'c':
				type = TYPE_CHAR;
				break;
			case 'f':
				type = TYPE_FLOAT;
				break;
			default:
				return count;
			}

			if (flags & FLAGS_IGN) {
				switch (type) {
				case TYPE_INT:
					__scan_int(stream, width, flags);
					break;
				case TYPE_UINT:
					__scan_uint(stream, width, flags);
					break;
				case TYPE_FLOAT:
					__scan_float(stream, flags);
					break;
				case TYPE_STR:
					free(__scan_str(stream, width, flags));
					break;
				case TYPE_CHAR:
					__scan_char(stream, flags);
					break;
				}
			}
			else {
				switch (type) {
				case TYPE_INT:
					i = va_arg(ap, int*);
					*i = __scan_int(stream, width, flags);
					break;
				case TYPE_UINT:
					u = va_arg(ap, unsigned int*);
					*u = __scan_uint(stream, width, flags);
					break;
				case TYPE_FLOAT:
					if (flags & FLAGS_LONG) {
						lf = va_arg(ap, double*);
						*lf = __scan_float(stream, flags);
					}
					else if (flags & FLAGS_LLONG) {
						Lf = va_arg(ap, long double*);
						*Lf = __scan_float(stream, flags);
					}
					else {
						f = va_arg(ap, float*);
						*f = __scan_float(stream, flags);
					}
					break;
				case TYPE_STR:
					s = va_arg(ap, char**);
					*s = __scan_str(stream, width, flags);
					break;
				case TYPE_CHAR:
					c = va_arg(ap, char*);
					*c = __scan_char(stream, flags);
					break;
				}
			}
			count++;
		}
		else {
			if (getc(stream) != *format) {
				return count;
			}
		}

		format++;
	}

	return count;
}
