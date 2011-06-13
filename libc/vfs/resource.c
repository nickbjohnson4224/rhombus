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

#include <stdlib.h>
#include <vfs.h>

/*****************************************************************************
 * resource_free
 *
 * Free a resource structure, including all allocated memory in standard 
 * fields.
 */

void resource_free(struct resource *r) {
	
	if (r->acl) {
		free(r->acl);
	}

	if (r->lock) {
		free(r->lock);
	}

	if (r->symlink) {
		free(r->symlink);
	}

	free(r);
}

/*****************************************************************************
 * resource_cons
 *
 * Create a new resource structure of type <type> with default permission
 * bitmap <perm>. Only the critical fields are set; all others are zeroed.
 */

struct resource *resource_cons(int type, int perm) {
	struct resource *r;

	r = calloc(sizeof(struct resource), 1);
	r->type = type;
	r->acl  = acl_set_default(NULL, perm);

	if (FS_IS_DIR(type)) {
		r->vfs = calloc(sizeof(struct vfs_node), 1);
		r->vfs->resource = r;
	}

	return r;
}
