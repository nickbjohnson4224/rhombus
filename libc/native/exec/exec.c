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
#include <natio.h>
#include <errno.h>
#include <arch.h>
#include <exec.h>
#include <page.h>
#include <pack.h>

/****************************************************************************
 * bootstrap
 *
 * Exec-persistent address where the executable image is to be loaded.
 */

static uint8_t *bootstrap = (void*) ESPACE;

/****************************************************************************
 * _save
 *
 * Save all important (standard) file descriptors and command line arguments 
 * to the dictionary for later retrieval.
 */

void _save(const char **argv) {
	int argc;

	if (stdin)  fdsave(0,  stdin->fd);
	if (stdout) fdsave(1, stdout->fd);
	if (stderr) fdsave(2, stderr->fd);
	fdsave(3, fs_root);

	if (argv) {
		for (argc = 0; argv[argc]; argc++);

		argv_pack(argc, argv);
	}

	__saveenv();

	__pack_save();
}

/****************************************************************************
 * execiv
 *
 * Execute with executable pointer and argument list.
 */

int execiv(uint8_t *image, size_t size, char const **argv) {

	page_anon(bootstrap, size, PROT_READ | PROT_WRITE);
	memcpy(bootstrap, image, size);

	_save(argv);

	if (_exec()) {
		errno = ENOEXEC;
		return -1;
	}

	return 0;
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
	char *fullpath;

	fullpath = strvcat(getenv("PATH"), "/", path, NULL);

	image = fopen(fullpath, "r");

	if (!image) {
		return -1;
	}

	fseek(image, 0, SEEK_END);
	size = ftell(image);

	if (!size) {
		errno = ENOEXEC;
		return -1;
	}

	page_anon(bootstrap, size, PROT_READ | PROT_WRITE);

	rewind(image);
	if (size != fread(bootstrap, sizeof(char), size, image)) {
		errno = ENOEXEC;
		return -1;
	}

	_save(argv);

	if (_exec()) {
		errno = ENOEXEC;
		return -1;
	}

	return 0;
}
