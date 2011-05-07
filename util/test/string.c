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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "test.h"

int test_string(void) {
	return 0
		+ test_memchr()
		+ test_memcmp()
		+ test_memcpy()
		+ test_memmove()
		+ test_memset()
		+ test_memclr()
		+ test_strcpy()
		+ test_strncpy()
		+ test_strcat()
		+ test_strncat()
		+ test_strcmp()
		+ test_strncmp()
		+ test_strlen()
		+ test_strchr()
		+ test_strrchr()
		+ test_strstr()
		+ test_strpbrk()
		+ test_strspn()
		+ test_strcspn()
		+ test_strtok()
		+ test_strdup()
		+ test_strparse()
	;
}

int test_memchr(void) {
	const char *s = "";
	
	printf("\tmemchr");

	if (*(char*) memchr("", '\0', 1) != '\0') return 1;
	if (*(char*) memchr("123", '3', 3) != '3') return 1;
	if (memchr(s, 'a', 0) != NULL) return 1;
	if (memchr(s, 'a', 1) != NULL) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_memcmp(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char value1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11};

	printf("\tmemcmp");

	if (memcmp(value0, value1, 9) != 0) return 1;
	if (memcmp(value0, value1, 10) == 0) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_memcpy(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	char value1[10] = {11, 11, 11, 11, 11, 11, 11, 11, 11, 11};

	printf("\tmemcpy");

	memcpy(value1, value0, 5);
	if (value0[5] == value1[5]) return 1;
	memcpy(value1, value0, 10);
	if (memcmp(value1, value0, 10)) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_memmove(void) {
	char value0[10] = {1, 2, 3, 4, 5, 6, 7, 8 ,9, 10};
	char value1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	printf("\tmemmove");

	memmove(&value0[0], &value0[3], 7);
	if (value0[0] != 4) return 1;
	if (value0[6] != 10) return 1;
	memmove(&value1[3], &value1[0], 7);
	if (value1[0] != 1) return 1;
	if (value1[3] != 1) return 1;
	if (value1[9] != 7) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_memset(void) {
	char value0[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	
	printf("\tmemset");

	memset(value0, 2, 5);
	if (value0[0] != 2) return 1;
	if (value0[4] != 2) return 1;
	if (value0[5] != 1) return 1;
	memset(value0, 1, 0);
	if (value0[0] != 2) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_memclr(void) {
	char value0[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	printf("\tmemclr");

	memclr(value0, 5);
	if (value0[0] != 0) return 1;
	if (value0[4] != 0) return 1;
	if (value0[5] != 1) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strcpy(void) {
	const char *str = "string";
	char buffer[11] = "aaaaaaaaaa";

	printf("\tstrcpy");

	strcpy(buffer, str);
	if (strcmp(buffer, str)) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strstr(void) {
	const char *needle = "needle";
	const char *haystack = " needl needle needle";
	
	printf("\tstrstr");

	if (strstr(haystack, needle) != &haystack[7]) return 1;
	if (strstr(haystack, "needlea") != NULL) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strncpy(void) {
	const char *str = "string";
	char buffer[11] = "aaaaaaaaaa";

	printf("\tstrncpy");
	
	strncpy(buffer, str, 5);
	if (buffer[5] != 'a') return 1;
	strncpy(buffer, str, 10);
	if (strcmp(buffer, str)) return 1;
	if (buffer[6] != '\0') return 1;
	if (buffer[10] != '\0') return 1;

	printf("\tpassed.\n");
	
	return 0;
}

int test_strcmp(void) {
	
	printf("\tstrcmp");

	if (strcmp("abcdefg", "abcdefg") != 0) return 1;
	if (strcmp("abcdefg", "abcdef") <= 0) return 1;
	if (strcmp("abcdef", "abcdefg") >= 0) return 1;
	if (strcmp("abcdefg", "abcdeff") <= 0) return 1;
	if (strcmp("abcdeff", "abcdefg") >= 0) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strncmp(void) {
	
	printf("\tstrncmp");

	if (strncmp("abcdefg", "abcdefg", 7) != 0) return 1;
	if (strncmp("abcdefg", "abcdef", 7) <= 0) return 1;
	if (strncmp("abcdef", "abcdefg", 7) >= 0) return 1;
	if (strncmp("abcdefg", "abcdeff", 7) <= 0) return 1;
	if (strncmp("abcdeff", "abcdefg", 7) >= 0) return 1;
	if (strncmp("abcdeff", "abcdefg", 6) != 0) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strlen(void) {
	
	printf("\tstrlen");

	if (strlen("abcdefg") != 7) return 1;
	if (strlen("") != 0) return 1;
	if (strlen("abc") != 3) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strchr(void) {
	const char *str = "hello";

	printf("\tstrchr");

	if (strchr(str, 'l') != &str[2]) return 1;
	if (strchr(str, 'q') != NULL) return 1;
	if (strchr(str, '\0') != NULL) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strrchr(void) {
	const char *str = "hello";

	printf("\tstrrchr");

	if (strchr(str, 'l') != &str[2]) return 1;
	if (strchr(str, 'q') != NULL) return 1;
	if (strchr(str, '\0') != NULL) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strpbrk(void) {
	const char *str = "hello, world!";

	printf("\tstrpbrk");

	if (strpbrk(str, "l") != &str[2]) return 1;
	if (strpbrk(str, "w!") != &str[7]) return 1;
	if (strpbrk(str, "!w") != &str[7]) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strcat(void) {
	const char *str1 = "hello ";
	const char *str2 = "world!";
	char buffer[20];

	printf("\tstrcat");

	strcpy(buffer, str1);
	strcat(buffer, str2);

	if (strcmp(buffer, "hello world!")) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strncat(void) {
	const char *str1 = "hello ";
	const char *str2 = "world!";
	char buffer[20];

	printf("\tstrncat");

	strcpy(buffer, str1);
	strncat(buffer, str2, 20);

	if (strcmp(buffer, "hello world!")) return 1;
	strcpy(buffer, str1);
	strncat(buffer, str2, 4);
	if (buffer[10] != '\0') return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strspn(void) {
	
	printf("\tstrspn");

	if (strspn("aabbcc", "a") != 2) return 1;
	if (strspn("aabbcc", "ab") != 4) return 1;
	if (strspn("aabbcc", "ba") != 4) return 1;
	if (strspn("ababcc", "ba") != 4) return 1;
	if (strspn("aabbcc", "abc") != 6) return 1;
	if (strspn("aabbcc", "cab") != 6) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strcspn(void) {
	
	printf("\tstrcspn");

	if (strcspn("aabbcc", "bca") != 0) return 1;
	if (strcspn("aabbcc", "bc") != 2) return 1;
	if (strcspn("aabbcc", "cb") != 2) return 1;
	if (strcspn("aabbcc", "c") != 4) return 1;
	if (strcspn("aabbcc", "d") != 6) return 1;

	printf("\tpassed.\n");
	
	return 0;
}

int test_strtok(void) {
	char str[18];
	strcpy(str, "abc def ab:de");

	printf("\tstrtok");

	if (strcmp(strtok(str, " "), "abc")) return 1;
	if (strcmp(strtok(NULL, ": "), "def")) return 1;
	if (strcmp(strtok(NULL, "d:"), "ab")) return 1;
	if (strcmp(strtok(NULL, " "), "de")) return 1;

	printf("\tpassed.\n");

	return 0;
}

int test_strdup(void) {
	char *str;
	
	printf("\tstrdup");

	str = strdup("supercalifragilistic");
	if (!str) return 1;
	if (strcmp(str, "supercalifragilistic")) return 1;
	free(str);

	printf("\tpassed.\n");

	return 0;
}

int test_strparse(void) {
	char **strv;

	printf("\tstrparse");

	strv = strparse("abc def:ghi alphabet-rhombus", " :");
	if (!strv) return 1;
	if (!strv[3] || strv[4]) return 1;
	if (strcmp(strv[0], "abc")) return 1;
	if (strcmp(strv[1], "def")) return 1;
	if (strcmp(strv[2], "ghi")) return 1;
	if (strcmp(strv[3], "alphabet-rhombus")) return 1;

	printf("\tpassed.\n");

	return 0;
}
