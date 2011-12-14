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

#include <rho/layout.h>
#include <rho/natio.h>
#include <rho/arch.h>
#include <rho/exec.h>
#include <rho/page.h>

/****************************************************************************
 * execiv
 *
 * Execute with executable pointer and argument list.
 */

int execiv(uint8_t *image, size_t size, char const **argv) {
	extern char **environ;
	const char *depname;
	rp_t *fdtab_pack;
	char *pack;
	char *deppath;
	void *pack_region;
	void *object;
	uint32_t i;

	if (!image) {
		errno = ENOENT;
		return -1;
	}

	/* save standard streams and filesystem root */
	pack_region = sltalloc("libc.fdtab", sizeof(rp_t) * 4);
	page_anon(pack_region, sizeof(rp_t) * 4, PROT_READ | PROT_WRITE);
	fdtab_pack = (void*) pack_region;
	fdtab_pack[0] = fd_rp(0);
	fdtab_pack[1] = fd_rp(1);
	fdtab_pack[2] = fd_rp(2);
	fdtab_pack[3] = fs_root;

	/* save argument list */
	if (argv) {
		pack = packarg(argv);
		pack_region = sltalloc("libc.argv", msize(pack));
		page_anon(pack_region, msize(pack), PROT_READ | PROT_WRITE);
		memcpy(pack_region, pack, msize(pack));
		free(pack);
	}

	/* save environment variables */
	pack = packenv((const char **) environ);
	if (pack) {
		pack_region = sltalloc("libc.env", msize(pack));
		page_anon(pack_region, msize(pack), PROT_READ | PROT_WRITE);
		memcpy(pack_region, pack, msize(pack));
		free(pack);
	}

	/* pull all dependencies */
	if (getppid() != 1) { // XXX - hack to prevent this from running on init
		for (i = 0;; i++) {
			depname = dl->dep(image, i, 0);
			if (!depname) break;

			deppath = strvcat("/lib/", depname, NULL);
			object = dlopen(deppath, RTLD_LAZY | RTLD_GLOBAL | RTLD_IMAGE);
			free(deppath);
		}
	}

	if (dl->exec(image, size, 0)) {
		errno = ENOEXEC;
		return -1;
	}

	return 0;
}

/****************************************************************************
 * exec*
 *
 * Execute.
 */

int execi(uint8_t *image, size_t size) {
	return execiv(image, size, NULL);
}

int execv(const char *path, char const **argv) {
	void *image;

	image = load_exec(path);
	return execiv(image, msize(image), argv);
}

int exec(const char *path) {
	const char *argv[] = { path, NULL };
	return execv(path, argv);
}
