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

#include <string.h>
#include <stdio.h>

#include "test.h"

int test_string(void) {
	return 0
		+ test_memchr()
		+ test_memcmp()
		+ test_memcpy()
		+ test_memmove()
//		+ test_memset()
//		+ test_memclr()
//		+ test_strcpy()
//		+ test_strncpy()
//		+ test_strcat()
//		+ test_strncat()
//		+ test_strcmp()
//		+ test_strncmp()
//		+ test_strlen()
//		+ test_strchr()
//		+ test_strrchr()
//		+ test_strstr()
//		+ test_strpbrk()
//		+ test_strspn()
//		+ test_strcspn()
//		+ test_strtok()
	;
}

int test_memchr(void) {
	const char *s = "";
	
	printf("\tmemchr");

	if (*(char*) memchr("", '\0', 1) != '\0') return 1;
	else printf(".");
	
	if (*(char*) memchr("123", '3', 3) != '3') return 1;
	else printf(".");

	if (memchr(s, 'a', 0) != s) return 1;
	else printf(".");

	if (memchr(s, 'a', 1) != &s[1]) return 1;
	else printf(".");

	printf(" passed.\n");

	return 0;
}

int test_memcmp(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char value1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11};

	printf("\tmemcmp");

	if (memcmp(value0, value1, 9) != 0) return 1;
	else printf(".");

	if (memcmp(value0, value1, 10) == 0) return 1;
	else printf(".");

	printf(" passed.\n");

	return 0;
}

int test_memcpy(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char value1[10] = {11, 11, 11, 11, 11, 11, 11, 11, 11, 11};

	printf("\tmemcpy");

	memcpy(value1, value0, 5);
	if (value0[5] == value1[5]) return 1;
	else printf(".");

	memcpy(value1, value0, 10);
	if (memcmp(value1, value0, 10)) return 1;
	else printf(".");

	printf(" passed.\n");

	return 0;
}

int test_memmove(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8 ,9, 10};
	char value1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	printf("\tmemmove");

	memmove(&value0[0], &value0[3], 7);
	if (value0[0] != 4) return 1;
	else printf(".");

	if (value0[6] != 10) return 1;
	else printf(".");

	memmove(&value1[3], &value1[0], 7);
	if (value1[0] != 1) return 1;
	else printf(".");

	if (value1[3] != 1) return 1;
	else printf(".");

	if (value1[9] != 7) return 1;
	else printf(".");

	printf(" passed.\n");

	return 0;
}
