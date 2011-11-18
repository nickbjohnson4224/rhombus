/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repan <jrepan at gmail.com>
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <rho/natio.h>
#include <rho/arch.h>
#include <rho/exec.h>
#include <rho/page.h>
#include <rho/pack.h>

/****************************************************************************
 * execiv
 *
 * Execute with executable pointer and argument list.
 */

int execiv(uint8_t *image, size_t size, char const **argv) {
	struct dl_list *list;
	char *argv_pack;

	if (!image) {
		errno = ENOENT;
		return -1;
	}

	/* build list for linker */
	list = malloc(sizeof(struct dl_list) * 1);

	list[0].type = DL_EXEC;
	list[0].base = image;
	list[0].size = size;
	list[0].name[0] = '\0';

	/* save standard streams and filesystem root */
	fdsave(0, fd_rp(0));
	fdsave(1, fd_rp(1));
	fdsave(2, fd_rp(2));
	fdsave(3, fs_root);

	/* save argument list */
	if (argv) {
		argv_pack = packarg(argv);
		__pack_add(PACK_KEY_ARG, argv_pack, msize(argv_pack));
		free(argv_pack);
	}

	/* save environment variables */
	__saveenv();

	/* persist saved stuff */
	__pack_save();

	if (dl_exec(list, 1)) {
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
	void *image;

	image = load_exec(path);
	return execiv(image, msize(image), argv);
}

int exec(const char *path) {
	const char *argv[] = { path, NULL };
	return execv(path, argv);
}
