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

#include <string.h>
#include <stddef.h>
#include <natio.h>
#include <proc.h>

size_t lfs_list_dir(char *buffer, size_t size, struct lfs_node *dir) {
	struct lfs_node *child;

	child = dir->daughter;

	if (!child) {
		strlcpy(buffer, "", size);
		return 0;
	}

	strlcpy(buffer, child->name, size);
	child = child->sister1;
	while (child) {
		strlcat(buffer, ":", size);
		strlcat(buffer, child->name, size);
		child = child->sister1;
	}

	return strlen(buffer);
}
