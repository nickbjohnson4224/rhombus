/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch.h>
#include <exec.h>
#include <mmap.h>
#include <io.h>

static uint8_t *bootstrap = (void*) ESPACE;

static struct {
	size_t argc;
	char *argv[100];
	char argvv[100][100];
} *argv_table = (void*) ARGV_TABLE;

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

int execiv(uint8_t *image, size_t size, char const **argv) {

	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	memcpy(bootstrap, image, size);

	_argv_copy(argv);

	return _exec((uintptr_t) bootstrap);
}

int execi(uint8_t *image, size_t size) {
	return execiv(image, size, NULL);
}

int execv(const char *path, char const **argv) {
	int file;
	size_t size;
	char buffer[20];

	file = find(path);

	if (file == -1) {
		return 1;
	}

	info(file, buffer, "size");

	size = atoi(buffer);

	if (!size) {
		return 2;
	}

	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	read(file, bootstrap, size, 0);

	_argv_copy(argv);

	return _exec((uintptr_t) bootstrap);
}
