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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mmap.h>

#include "test.h"

int test_heap(void) {
	return 0
		+ test_calloc()
		+ test_valloc()
		+ test_malloc()
//		+ test_aalloc()
		+ test_realloc()
//		+ test_heap_torture()
	;
}

int test_malloc(void) {
	char *block, *block1;

	printf("\tmalloc");

	block = malloc(0);
	if (block == NULL) {
		printf(".");
	}
	else {
		free(block);
		printf(".");
	}

	block = malloc(127);
	if (block == NULL) return 1;
	else printf(".");

	block1 = malloc(127);
	if ((block1 == NULL) || (block1 == block)) return 1;
	else printf(".");

	block[0] = 0;
	block[126] = 0;
	printf(".");

	if (msize(block) < 127) return 1;
	else printf(".");

	free(block);
	printf(".");

	free(block1);
	printf(".");

	free(malloc(127));
	printf(".");

	printf(" passed.\n");

	return 0;
}

int test_calloc(void) {
	char *block;

	printf("\tcalloc");

	block = calloc(17, 5);
	if (block[0] || block[84]) return 1;
	else printf(".");

	if (msize(block) < 85) return 1;
	else printf(".");

	free(block);
	printf(".");

	printf(" passed.\n");

	return 0;
}

int test_valloc(void) {
	char *block;

	printf("\tvalloc");

	block = valloc(1000);
	if (msize(block) < 1000) return 1;
	else printf(".");

	if ((uintptr_t) block % PAGESZ) return 1;
	else printf(".");

	free(block);
	printf(".");

	block = valloc(PAGESZ * 10);
	if (msize(block) < PAGESZ * 10) return 1;
	else printf(".");

	if ((uintptr_t) block % PAGESZ) return 1;

	free(block);
	printf(".");

	printf(" passed.\n");

	return 0;
}

int test_realloc(void) {
	const char *value = "hello, world!";
	char *block;

	printf("\trealloc");

	block = malloc(strlen(value) + 1);
	strcpy(block, value);

	block = realloc(block, 100);

	if (block == NULL) return 1;
	else printf(".");

	if (msize(block) < 100) return 1;
	else printf(".");

	if (strcmp(block, value)) return 1;
	else printf(".");

	printf(" passed.\n");

	return 0;
}
