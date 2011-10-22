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

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define FLAG_SIGN		0x01	// Always show sign
#define FLAG_LEFT		0x02	// Left align
#define FLAG_ALT		0x04	// Alternate form
#define FLAG_ZERO		0x08	// Zero-pad
#define FLAG_UPPER		0x10	// Upper case
#define FLAG_OCTAL		0x20	// Octal output
#define FLAG_HEX		0x40	// Hexadecimal output
#define FLAG_EXP		0x80	// Use scientific notation
#define FLAG_MEXP		0x100	// Maybe use scientific notation

#define TYPE_STRING		0		// String
#define TYPE_CHAR		1		// Character
#define TYPE_LITERAL	2		// Literal '%"
#define TYPE_INT		3		// Signed integer
#define TYPE_UINT		4		// Unsigned integer
#define TYPE_DOUBLE		5		// Floating point
#define TYPE_RP			6		// Resource pointer

#define LENGTH_BYTE		0		// char
#define LENGTH_SHORT	1		// short int
#define LENGTH_LONG		3		// int or double
#define LENGTH_LLONG	4		// long long int or long double

static char *__format_uint(unsigned int value, int flags) {
	char buffer[16];
	int i;
	int base;

	const char *digits_lower = "0123456789abcdef";
	const char *digits_upper = "0123456789ABCDEF";
	const char *digits;

	if (flags & FLAG_OCTAL) 	base = 8;
	else if (flags & FLAG_HEX) 	base = 16;
	else 						base = 10;

	if (value == 0) {
		return strdup("0");
	}

	if (flags & FLAG_UPPER) {
		digits = digits_upper;
	}
	else {
		digits = digits_lower;
	}

	buffer[15] = '\0';
	for (i = 14; i >= 0; i--) {
		if (!value) break;
		buffer[i] = digits[value % base];
		value /= base;
	}

	if (flags & FLAG_SIGN) {
		buffer[i--] = '+';
	}

	i++;

	if (flags & FLAG_ALT) {
		if (flags & FLAG_OCTAL) {
			return strvcat("0", &buffer[i], NULL);
		}
		if (flags & FLAG_HEX) {
			return strvcat((flags & FLAG_UPPER) ? "0X" : "0x", &buffer[i], NULL);
		}
	}
	return strdup(&buffer[i]);
}

static char *__format_int(int value, int flags) {
	char *string, *temp;

	if (value < 0) {
		temp = __format_uint(-value, flags);
		string = strvcat("-", temp, NULL);
		free(temp);
	}
	else {
		string = __format_uint(value, flags);
	}

	return string;
}
	
static char *__format_double_frac(long double value, int flags, int precision) {
	char *string;
	int i;

	if (value < 0) value = -value;
	if (value > 1) value -= floorl(value);

	string = malloc(precision + 1);

	for (i = 0; i < precision; i++) {
		value *= 10;
		string[i] = (int) value + '0';
		value -= floorl(value);

		if (value == 0.0 && !(flags & FLAG_ALT && flags & FLAG_MEXP)) {
			i++;
			break;
		}
	}
	
	string[i] = '\0';

	return string;
}

static char *__format_double_int(long double value, int flags) {
	char *temp, *string;
	int expt;

	expt = log10l(value) + 1;
	value /= (int) powl(10, expt);

	temp = __format_double_frac(value, flags | FLAG_ALT | FLAG_MEXP, expt);

	if (value < 0) {
		string = strvcat("-", temp, NULL);
		free(temp);
	}
	else if (flags & FLAG_SIGN) {
		string = strvcat("+", temp, NULL);
		free(temp);
	}
	else {
		string = temp;
	}

	return string;
}

static char *__format_double(long double value, int flags, int precision) {
	char *int_part;
	char *dec_part;
	char *string;
	long double i;

	if (isnan(value)) {
		if (flags & FLAG_UPPER) {
			return strdup("NAN");
		}
		else {
			return strdup("nan");
		}
	}

	if (isinf(value)) {
		if (flags & FLAG_UPPER) {
			if (value < 0) {
				return strdup("-INF");
			}
			else {
				if (flags & FLAG_SIGN) {
					return strdup("+INF");
				}
				else {
					return strdup("INF");
				}
			}
		}
		else {
			if (value < 0) {
				return strdup("-inf");
			}
			else {
				if (flags & FLAG_SIGN) {
					return strdup("+inf");
				}
				else {
					return strdup("inf");
				}
			}
		}
	}

	if (flags & FLAG_EXP || (flags & FLAG_MEXP && value > 1000000000)) {
		i = log10l(value);
		value /= powl(10, floorl(i));

		int_part = __format_double(value, flags & ~(FLAG_EXP), precision);
		dec_part = __format_int(i, flags | FLAG_SIGN);

		string = strvcat(int_part, (flags & FLAG_UPPER) ? "E" : "e", dec_part, NULL);

		free(int_part);
		free(dec_part);
	}
	else {
		value = modfl(value, &i);

		if (value > 2000000000) {
			int_part = __format_double_int(i, flags);
		}
		else {
			int_part = __format_int(i, flags);
		}
		dec_part = __format_double_frac(value, flags, precision);

		if (i == 0.0 && value < 0) {
			if (value != 0.0 || flags & FLAG_ALT) {
				string = strvcat("-", int_part, ".", dec_part, NULL);
			}
			else {
				string = strvcat("-", int_part, NULL);
			}
		}
		else {
			if (value != 0.0 || flags & FLAG_ALT) {
				string = strvcat(int_part, ".", dec_part, NULL);
			}
			else {
				string = strdup(int_part);
			}
		}

		free(int_part);
		free(dec_part);
	}

	return string;
}

char *__format(const char **_fmt, va_list *argp) {
	int flags, type, length;
	size_t width, precision;
	const char *fmt = *_fmt;
	char *string;
	char *string1;

	uint64_t val_ll;
	long double val_d;
	int val_i;
	unsigned int val_u;
	const char *val_s;
	char val_c;

	flags = 0;
	type  = 0;
	width = 0;
	length = LENGTH_LONG;
	precision = 6;

	/* check for '%' */
	if (*fmt != '%') {
		return NULL;
	}

	/* read flags */
	do {
		fmt++;
		switch (*fmt) {
		case '+': flags |= FLAG_SIGN;  continue;
		case '-': flags |= FLAG_LEFT;  continue;
		case '#': flags |= FLAG_ALT;   continue;
		case '0': flags |= FLAG_ZERO;  continue;
		case '\0': return NULL;
		}
		break;
	} while (1);

	/* read width */
	if (*fmt == '*') {
		width = va_arg(*argp, int);
		fmt++;
	}
	else if (isdigit(*fmt)) {
		width = atoi(fmt);
		while (isdigit(*fmt)) fmt++;
	}

	/* read precision */
	if (*fmt == '.') {
		fmt++;

		if (*fmt == '*') {
			precision = va_arg(*argp, int);
			fmt++;
		}
		else if (isdigit(*fmt)) {
			precision = atoi(fmt);
			while (isdigit(*fmt)) fmt++;
		}
		else {
			return NULL;
		}
	}

	/* read length */
	switch (*fmt) {
	case 'h':
		fmt++;
		if (*fmt == 'h') {
			length = LENGTH_BYTE;
			fmt++;
		}
		else {
			length = LENGTH_SHORT;
		}
		break;
	case 'l':
		fmt++;
		if (*fmt == 'l') {
			length = LENGTH_LONG;
			fmt++;
		}
		else {
			length = LENGTH_LLONG;
		}
		break;
	case 'L': case 'j':
		length = LENGTH_LLONG;
		fmt++;
		break;
	case 'z': case 't':
		length = LENGTH_LONG;
		fmt++;
	case '\0':
		return NULL;
	}

	/* read type */
	switch (*fmt) {
	case 'i': case 'd':
		type = TYPE_INT;
		break;
	case 'u':
		type = TYPE_UINT;
		break;
	case 'F': flags |= FLAG_UPPER;
	case 'f':
		type = TYPE_DOUBLE;
		break;
	case 'G': flags |= FLAG_UPPER;
	case 'g':
		type = TYPE_DOUBLE;
		flags |= FLAG_MEXP;
		break;
	case 'E': flags |= FLAG_UPPER;
	case 'e':
		type = TYPE_DOUBLE;
		flags |= FLAG_EXP;
		break;
	case 'X': case 'p': flags |= FLAG_UPPER;
	case 'x':
		type = TYPE_UINT;
		flags |= FLAG_HEX;
		break;
	case 'O': flags |= FLAG_UPPER;
	case 'o':
		type = TYPE_UINT;
		flags |= FLAG_OCTAL;
		break;
	case 's':
		type = TYPE_STRING;
		break;
	case 'c':
		type = TYPE_CHAR;
		break;
	case 'r':
		type = TYPE_RP;
		break;
	case '%':
		type = TYPE_LITERAL;
		break;
	default:
		return NULL;
	}

	/* read value */
	switch (type) {
	case TYPE_STRING:	val_s = va_arg(*argp, const char *); 	break;
	case TYPE_CHAR:		val_c = va_arg(*argp, char); 			break;
	case TYPE_LITERAL:	val_c = '%';							break;
	case TYPE_INT:		val_i = va_arg(*argp, int);				break;
	case TYPE_UINT:		val_u = va_arg(*argp, unsigned int);	break;
	case TYPE_DOUBLE:
		if (length == LENGTH_LLONG)	val_d = va_arg(*argp, long double);
		else						val_d = va_arg(*argp, double);
		break;
	case TYPE_RP:		val_ll = va_arg(*argp, uint64_t);		break;
	}

	/* format value */
	switch (type) {
	case TYPE_STRING:
		if (!val_s) string = strdup("(null)");
		else string = strdup(val_s);
		break;
	case TYPE_CHAR: case TYPE_LITERAL:
		string = malloc(sizeof(char) * 2);
		string[0] = val_c;
		string[1] = '\0';
		break;
	case TYPE_INT:
		string = __format_int(val_i, flags);
		break;
	case TYPE_UINT:
		string = __format_uint(val_u, flags);
		break;
	case TYPE_DOUBLE:
		string = __format_double(val_d, flags, precision);
		break;
	case TYPE_RP:
		string = rtoa(val_ll);
		break;
	}

	if (strlen(string) < width) {
		string1 = malloc(sizeof(char) * (width + 1));

		if (flags & FLAG_LEFT) {
			strlcpy(string1, string, width + 1);

			if (flags & FLAG_ZERO) {
				strlcat(string1, "000000000000000000000000", width + 1);
			}
			else {
				strlcat(string1, "                        ", width + 1);
			}
		}
		else {
			if (flags & FLAG_ZERO) {
				strlcpy(string1, "000000000000000000000000", width - strlen(string) + 1);
			}
			else {
				strlcpy(string1, "                        ", width - strlen(string) + 1);
			}

			strlcat(string1, string, width + 1);
		}

		free(string);
	}
	else {
		string1 = string;
	}

	*_fmt = fmt;
	return string1;
}
