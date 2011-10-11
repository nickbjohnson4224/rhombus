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

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static void _string_adds(size_t *length, size_t *top, char **string, const char *s);
static void _string_addc(size_t *length, size_t *top, char **string, char c);

/****************************************************************************
 * vsaprintf
 *
 * Prints a formatted string to an allocated buffer, then returns the
 * buffer. Returns null on error.
 */

char *vsaprintf(const char *format, va_list ap) {
	const char *format_tmp;
	char *temp;
	int i;

	char *string = NULL;
	size_t length;
	size_t top;

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			format_tmp = &format[i];
			temp = __format(&format_tmp, &ap);
			_string_adds(&length, &top, &string, temp);
			free(temp);
			i = (uintptr_t) format_tmp - (uintptr_t) format;
		}
		else {
			_string_addc(&length, &top, &string, format[i]);
		}
	}

	return string;
}

/*****************************************************************************
 * _string_adds
 *
 * Add a string <s> to the string buffer <*string>, which is of length 
 * <*length> and has null byte at index <*top>.
 */

static void _string_adds(size_t *length, size_t *top, char **string, const char *s) {
	char *old_string;
	size_t length_s;
	size_t length2;
	char *string2;
	
	length_s = strlen(s);

	if (!*string) {
		*string = strdup(s);
		*length = length_s + 1;
		*top    = length_s;
	}
	else {
		if (*top + length_s >= *length) {

			// reallocate string
			length2 = *length * 2 + length_s;
			string2 = malloc(length2);

			// copy to new string and concatenate
			strcpy(string2, *string);

			// free old string
			old_string = *string;
			*string = string2;
			*length = length2;
			free(old_string);
		}
		
		// concatenate strings
		strcpy(&(*string)[*top], s);
		*top += length_s;
	}
}

/*****************************************************************************
 * _string_addc
 *
 * Add a character <c> to the string buffer <*string>, which is of length 
 * <*length> and has null byte at index <*top>.
 */

static void _string_addc(size_t *length, size_t *top, char **string, char c) {
	char *old_string;
	size_t length2;
	char *string2;

	if (!*string) {
		*string = malloc(16);
		*length = 16;
		*top = 1;
		(*string)[0] = c;
		(*string)[1] = '\0';
	}
	else {
		if (*top + 1 >= *length) {
			
			// reallocate string
			length2 = *length * 2 + 1;
			string2 = malloc(length2);

			// copy to new string
			memcpy(string2, *string, *length);

			// free old string
			old_string = *string;
			*string = string2;
			*length = length2;
			free(old_string);
		}

		// add character
		(*string)[*top] = c;
		(*string)[*top+1] = '\0';
		*top += 1;
	}
}
