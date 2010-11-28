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

#include <driver.h>
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * inode_lookup
 *
 * Inode lookup hashtable (using chaining). Used to find filesystem objects
 * by inode alone.
 */

static struct fs_obj *inode_lookup[256];
static bool m_inode_lookup;

/*****************************************************************************
 * lfs_lookup
 *
 * Find a local filesystem object by inode number. Returns the found 
 * filesystem object on success, NULL on error.
 */

struct fs_obj *lfs_lookup(uint32_t inode) {
	struct fs_obj *obj;

	mutex_spin(&m_inode_lookup);

	obj = inode_lookup[inode % 256];

	while (obj) {
		if (obj->inode == inode) {
			mutex_free(&m_inode_lookup);
			return obj;
		}
		obj = obj->next;
	}

	mutex_free(&m_inode_lookup);

	return NULL;
}

/*****************************************************************************
 * lfs_root
 *
 * Changes the root of the local filesystem tree. In reality, all this does
 * is set the lookup for inode 0 to be this filesystem object.
 */

void lfs_root(struct fs_obj *root) {
	
	mutex_spin(&m_inode_lookup);

	root->inode = 0;
	root->next = inode_lookup[0];
	root->prev = NULL;
	inode_lookup[0] = root;

	mutex_free(&m_inode_lookup);
}

/*****************************************************************************
 * lfs_find
 *
 * Finds a file, starting at the filesystem object with inode <inode> in the
 * current driver. Returns a file structure referring to the found file, which
 * may not be in the current driver, on success, and NULL on failure. If 
 * <nolink> is true, terminal links are not followed, so link objects can be
 * found.
 */

uint64_t lfs_find(uint32_t inode, const char *path_str, bool nolink) {
	struct path   *path;
	struct fs_obj *fobj;
	struct fs_obj *sub;
	char *name;

	path = path_cons(path_str);
	fobj = lfs_lookup(inode);

	while (fobj) {
		name = path_next(path);

		if (!name) {
			if (fobj->link && !nolink) {
				return fobj->link;
			}
			else {
				return (((uint64_t) getpid() << 32) | fobj->inode);
			}
		}

		if (fobj->type != FOBJ_DIR) {
			free(name);
			return 0;
		}
		else {
			if (fobj->link) {
				free(name);
				path_prev(path);
				return fs_find(fobj->link, path_tail(path));
			}

			if ((acl_get(fobj->acl, gettuser()) & ACL_READ) == 0) {
				return 0;
			}

			sub = fobj->daughter;

			while (sub) {
				if (!strcmp(sub->name, name)) {
					break;
				}

				sub = sub->sister1;
			}

			free(name);

			if (sub) {
				fobj = sub;
			}
			else {
				return 0;
			}
		}
	}

	return 0;
}

/*****************************************************************************
 * lfs_push
 *
 * Add the filesystem object <obj> to the directory <dir>, giving it the
 * name <name>. Also adds the filesystem object <obj> to the inode lookup 
 * hash. Calls the active driver's push function and returns zero on success; 
 * returns nonzero on error.
 */

int lfs_push(struct fs_obj *dir, struct fs_obj *obj, const char *name) {
	struct fs_obj *sister;

	if (!(dir && obj && name)) {
		return 1;
	}

	mutex_spin(&dir->mutex);

	obj->name     = strdup(name);
	obj->mutex    = 0;
	obj->mother   = dir;
	obj->daughter = NULL;
	
	sister = dir->daughter;

	if (!sister) {
		/* only in the list, insert at top */
		dir->daughter = obj;
		obj->sister0 = NULL;
		obj->sister1 = NULL;
	}
	else {
		while (sister->sister1) {
			if (strcmp(sister->name, name) > 0) {
				break;
			}
			sister = sister->sister1;
		}

		/* insert */
		if (strcmp(sister->name, name) > 0) {
			if (sister->sister0) {
				sister->sister0->sister1 = obj;
			}
			else {
				dir->daughter = obj;
			}

			obj->sister0 = sister->sister0;
			obj->sister1 = sister;
			sister->sister0 = obj;
		}
		else {
			sister->sister1 = obj;
			obj->sister0 = sister;
			obj->sister1 = NULL;
		}
	}

	if (!obj->next) {
		obj->prev = NULL;
		obj->next = inode_lookup[obj->inode % 256];
		inode_lookup[obj->inode % 256] = obj;

		if (obj->next) {
			obj->next->prev = obj;
		}
	}

	mutex_free(&dir->mutex);

	if (active_driver->push) {
		return active_driver->push(obj);
	}
	else {
		return 0;
	}
}

/*****************************************************************************
 * lfs_pull
 *
 * Remove the filesystem object <obj> from its parent directory. Removes the 
 * filesystem object <obj> from the inode lookup hash. Calls the active 
 * driver's pull function and returns zero on success; returns nonzero on 
 * error.
 */

int lfs_pull(struct fs_obj *obj) {

	if (!obj) {
		return 1;
	}

	free(obj->name);
	
	if (obj->mother) {
		mutex_spin(&obj->mother->mutex);
		if (obj->mother->daughter == obj) {
			obj->mother->daughter = obj->sister1;
			if (obj->sister1) {
				obj->sister1->sister0 = NULL;
			}
		}
		else {
			if (obj->sister0) {
				obj->sister0->sister1 = obj->sister1;
			}
			if (obj->sister1) {
				obj->sister1->sister0 = obj->sister0;
			}
		}
		mutex_free(&obj->mother->mutex);
	}

	if (obj->next) {
		mutex_spin(&m_inode_lookup);
		if (obj->prev) {
			inode_lookup[obj->inode % 256] = obj->next;
		}
		if (obj->next) {
			obj->next->prev = obj->prev;
		}
		mutex_free(&m_inode_lookup);
	}
	
	if (active_driver->pull) {
		return active_driver->pull(obj);
	}
	else {
		return 0;
	}
}
