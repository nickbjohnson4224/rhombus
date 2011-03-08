/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include "fbterm.h"

#include <stdlib.h>
#include <driver.h>
#include <stdio.h>

int main(int argc, char **argv) {
	struct vfs_obj *root;
	uint64_t kbd;
	uint64_t fb;

	if (argc < 3) {
		fprintf(stderr, "%s: insufficient arguments\n", argv[0]);
		return 1;
	}

	kbd = io_find(argv[2]);
	fb  = io_find(argv[3]);

	if (!kbd) {
		fprintf(stderr, "%s: %s: keyboard not found\n", argv[0], argv[1]);
		return 1;
	}

	if (!fb) {
		fprintf(stderr, "%s: %s: graphics device not found\n", argv[0], argv[1]);
		return 1;
	}

	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);

	return 0;
}
