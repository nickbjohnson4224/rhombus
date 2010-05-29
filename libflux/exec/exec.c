/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/exec.h>
#include <flux/mmap.h>

static uint8_t *bootstrap = (void*) ESPACE;

static struct {
	size_t argc;
	char *argv[100];
	char argvv[100][100];
} *argv_table = (void*) ARGV_TABLE;

static void _strncpy(char *dest, char const *src, size_t n) {
	size_t i;

	for (i = 0; i < n - 1 && src[i]; i++) {
		dest[i] = src[i];
	}

	dest[i] = '\0';
}

int execiv(uint8_t *image, size_t size, char const **argv) {
	size_t i;

	mmap(argv_table, 0x10000, MMAP_READ | MMAP_WRITE);

	if (argv) {
		for (i = 0; argv[i]; i++) {
			argv_table->argv[i] = (char*) &argv_table->argvv[i];
			_strncpy(argv_table->argv[i], argv[i], 100);
		}

		argv_table->argv[i] = NULL;
		argv_table->argc = i;
	}
	else {
		argv_table->argc = 0;
		argv_table->argv[0] = NULL;
	}

	mmap(bootstrap, size, MMAP_READ | MMAP_WRITE);
	arch_memcpy(bootstrap, image, size);

	return _exec((uintptr_t) bootstrap);
}

int execi(uint8_t *image, size_t size) {
	return execiv(image, size, NULL);
}
