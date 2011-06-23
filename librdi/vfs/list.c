/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <rdi/vfs.h>

#include <string.h>
#include <stdlib.h>

/*****************************************************************************
 * vfs_list
 *
 * Returns a tab-separated list of directory entry names as a single string.
 * Returns null on error.
 *
 * This function does not acquire a lock on the directory, but the directory
 * and its entry structure may not be modified while this function is running.
 */

char *vfs_list(struct vfs_node *dir) {
	struct vfs_node *daughter;
	char *list = strdup("");
	char *old;

	if (!dir) {
		return NULL;
	}
	
	daughter = dir->daughter;
	while (daughter) {
		old = list;
		if (list[0]) list = strvcat(list, "\t", daughter->name, NULL);
		else list = strdup(daughter->name);
		free(old);
		daughter = daughter->sister1;
	}

	return list;
}
