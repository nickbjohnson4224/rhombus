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

static unsigned int __scan_uint(const char **_str, int width, int flags) {
	int base;
	unsigned int value;

	/* decide base */
	if (flags & FLAGS_DEC) base = 10;
	else if (flags & FLAGS_HEX) base = 16;
	else if (flags & FLAGS_OCT) base = 8;
	else {
		if (**_str == '0') {
			(*_str)++;
			width--;
			if (**_str == 'x' || **_str == 'X') {
				(*_str)++;
				width--;
				base = 16;
			}
			else {
				base = 8;
			}
		}
		else {
			base = 10;
		}
	}

	value = 0;
	while (__isbdigit(**_str, base)) {
		value *= base;
		value += __digit(**_str, base);
		(*_str)++;
		width--;

		if (width == 0) {
			return value;
		}
	}

	return value;
}

static int __scan_int(const char **_str, int width, int flags) {
	
	if (**_str == '-') {
		(*_str)++;
		return -__scan_uint(_str, width - 1, flags);
	}
	else {
		return __scan_uint(_str, width - 1, flags);
	}
}

static char *__scan_str(const char **_str, int width, int flags) {
	int count, i;
	char *string;

	for (count = 0; count < width; count++) {
		if (isspace((*_str)[count]) || !(*_str)[count]) break;
	}

	string = malloc(count + 1);

	for (i = 0; i < count; i++) {
		string[i] = **_str;
		(*_str)++;
	}

	string[i] = '\0';

	return string;
}

static char __scan_char(const char **_str, int flags) {
	char c;

	c = **_str;
	(*_str)++;

	return c;
}

static long double __scan_float(const char **_str, int flags) {
	return strtold(*_str, (char**) _str);
}

static void __scan_space(const char **_str) {
	while (isspace(**_str)) (*_str)++;
}

int vsscanf(const char *str, const char *format, va_list ap) {
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
			__scan_space(&str);
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
					__scan_int(&str, width, flags);
					break;
				case TYPE_UINT:
					__scan_uint(&str, width, flags);
					break;
				case TYPE_FLOAT:
					__scan_float(&str, flags);
					break;
				case TYPE_STR:
					free(__scan_str(&str, width, flags));
					break;
				case TYPE_CHAR:
					__scan_char(&str, flags);
					break;
				}
			}
			else {
				switch (type) {
				case TYPE_INT:
					i = va_arg(ap, int*);
					*i = __scan_int(&str, width, flags);
					break;
				case TYPE_UINT:
					u = va_arg(ap, unsigned int*);
					*u = __scan_uint(&str, width, flags);
					break;
				case TYPE_FLOAT:
					if (flags & FLAGS_LONG) {
						lf = va_arg(ap, double*);
						*lf = __scan_float(&str, flags);
					}
					else if (flags & FLAGS_LLONG) {
						Lf = va_arg(ap, long double*);
						*Lf = __scan_float(&str, flags);
					}
					else {
						f = va_arg(ap, float*);
						*f = __scan_float(&str, flags);
					}
					break;
				case TYPE_STR:
					s = va_arg(ap, char**);
					*s = __scan_str(&str, width, flags);
					break;
				case TYPE_CHAR:
					c = va_arg(ap, char*);
					*c = __scan_char(&str, flags);
					break;
				}
			}
			count++;
		}
		else {
			if (*str == *format) {
				str++;
				format++;
			}
			else {
				return count;
			}
		}

		format++;
	}

	return count;
}
