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

#include "dl.h"

void *dl_memcpy(void *dst, const void *src, size_t size) {
	size_t i;
	uint8_t *d = dst;
	const uint8_t *s = src;

	for (i = 0; i < size; i++) {
		d[i] = s[i];
	}

	return dst;
}

void *dl_memclr(void *ptr, size_t size) {
	size_t i;
	uint8_t *a = ptr;

	for (i = 0; i < size; i++) {
		a[i] = 0;
	}

	return ptr;
}

char *dl_strcpy(char *dst, const char *src) {
	size_t i;

	for (i = 0; src[i] != '\0'; i++) {
		dst[i] = src[i];
	}

	dst[i] = '\0';

	return dst;
}

int dl_strcmp(const char *s1, const char *s2) {
	size_t i;

	for (i = 0;; i++) {
		if (s1[i] == s2[i]) {
			if (s1[i] == '\0') return 0;
			continue;
		}
		if (s1[i] == '\0') return -1;
		if (s2[i] == '\0') return 1;
		if (s1[i] < s2[i]) return -1;
		else return 1;
	}
}

size_t dl_strlen(const char *str) {
	size_t i;

	for (i = 0; str[i]; i++);

	return i;
}
