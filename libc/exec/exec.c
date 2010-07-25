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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arch.h>
#include <exec.h>
#include <mmap.h>

/****************************************************************************
 * bootstrap
 *
 * Exec-persistent address where the executable image is to be loaded.
 */

static uint8_t *bootstrap = (void*) ESPACE;

/****************************************************************************
 * argv_table
 *
 * Exec-persistent structure storing arguments for the new program.
 */

static struct {
	size_t argc;
	char *argv[100];
	char argvv[100][100];
} *argv_table = (void*) ARGV_TABLE;

/****************************************************************************
 * _file_save
 *
 * Save all important (standard) file descriptors to the dictionary for
 * later retrieval.
 */

static void _file_save(void) {
	
	fsave("stdin", stdin);
	fsave("stdout", stdout);
	fsave("stderr", stderr);
	fsave("stdvfs", stdvfs);
}

/****************************************************************************
 * _argv_copy
 *
 * Save all arguments to argv_table for later retrieval.
 */

static void _argv_copy(char const **argv) {
	size_t i;

	mmap(argv_table, 0x10000, MMAP_READ | MMAP_WRITE);

	if (argv) {
		for (i = 0; argv[i]; i++) {
			argv_table->argv[i] = (char*) &argv_table->argvv[i];
			strlcpy(argv_table->argv[i], argv[i], 100);
		}

		argv_table->argv[i] = NULL;
		argv_table->argc = i;
	}
	else {
		argv_table->argc = 0;
		argv_table->argv[0] = NULL;
	}
}

/****************************************************************************
 * execiv
 *
 * Execute with executable pointer and argument list.
 */

int execiv(uint8_t *image, size_t size, char const **argv) {

	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	memcpy(bootstrap, image, size);

	_argv_copy(argv);
	_file_save();

	return _exec((uintptr_t) bootstrap);
}

/****************************************************************************
 * execi
 *
 * Execute with executable pointer.
 */

int execi(uint8_t *image, size_t size) {
	return execiv(image, size, NULL);
}

/****************************************************************************
 * execv
 *
 * Execute with executable path and argument list.
 */

int execv(const char *path, char const **argv) {
	FILE *image;
	size_t size;
	char buffer[20];

	image = fopen(path, "r");

	if (!image) {
		return 1;
	}

	info(image, buffer, "size");

	size = atoi(buffer);

	if (!size) {
		return 2;
	}

	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	read(image, bootstrap, size, 0);

	_argv_copy(argv);
	_file_save();

	return _exec((uintptr_t) bootstrap);
}
