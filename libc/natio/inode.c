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

#include <stdlib.h>
#include <string.h>
#include <natio.h>

static struct inode_list {
	struct inode_list *next;
	uint32_t inode;
	char *path;
} *_inode_list[0x100];

int lfs_add_inode(uint32_t inode, const char *path) {
	struct inode_list *l;

	l = malloc(sizeof(struct inode_list));
	
	if (!l) {
		return 1;
	}

	l->inode = inode;
	l->path = strdup(path);
	
	l->next = _inode_list[inode & 0xFF];
	_inode_list[inode & 0xFF] = l;

	return 0;
}

const char *lfs_get_inode(uint32_t inode) {
	struct inode_list *l;

	l = _inode_list[inode & 0xFF];

	while (l) {
		if (l->inode == inode) {
			break;
		}
		l = l->next;
	}

	if (l) {
		return l->path;
	}
	else {
		return NULL;
	}
}
