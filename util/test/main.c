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

#include "test.h"

int main(int argc, char **argv) {
	const char *testsuite;

	/* Test basic environment */
	printf("Testing basic environment:\n");

	printf("\tstrcmp"); {

		if (strcmp("", "") != 0) {
			printf("\nstrcmp(\"\", \"\") failed.\n");
			return 1;
		}
		else {
			printf(".");
		}

		if (strcmp("12345", "12345") != 0) {
			printf("\nstrcmp(\"12345\", \"12345\") failed.\n");
			return 1;
		}
		else {
			printf(".");
		}

		if (strcmp("12345", "123456") == 0) {
			printf("\nstrcmp(\"12345\", \"123456\") failed.\n");
			return 1;
		}
		else {
			printf(".");
		}

		if (strcmp("123456", "12345") == 0) {
			printf("\nstrcmp(\"123456\", \"12345\") failed.\n");
			return 1;
		}
		else {
			printf(".");
		}

		printf(" passed.\n");
	}

	printf("All tests passed.\n\n");

	if (argc == 1) {
		testsuite = "all";
	}
	else {
		testsuite = argv[1];
	}

	printf("Testsuite \"%s\" selected:\n", testsuite);

	if (!strcmp(testsuite, "string") || !strcmp(testsuite, "all")) {
		if (test_string()) {
			printf("string tests failed.\n");
			return 1;
		}
	}	
	
	if (!strcmp(testsuite, "heap") || !strcmp(testsuite, "all")) {
		if (test_heap()) {
			printf("heap tests failed.\n");
			return 1;
		}
	}

	return 0;
}
