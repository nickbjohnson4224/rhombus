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
 * may not be in the current driver, on success, and NULL on failure.
 */

uint64_t lfs_find(uint32_t inode, const char *path_str) {
	struct path   *path;
	struct fs_obj *fobj;
	struct fs_obj *sub;
	char *name;

	path = path_cons(path_str);
	fobj = lfs_lookup(inode);

	while (fobj) {
		name = path_next(path);

		if (!name) {
			if (fobj->type == FOBJ_LINK && fobj->link) {
				return fobj->link;
			}
			else {
				return (((uint64_t) getpid() << 32) | fobj->inode);
			}
		}

		if (fobj->type == FOBJ_LINK) {
			free(name);
			if (fobj->link) {
				path_prev(path);
				return fs_find(fobj->link, path_tail(path));
			}
			else {
				return 0;
			}
		}

		if (fobj->type != FOBJ_DIR) {
			free(name);
			return 0;
		}
		else {
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
 * lfs_list
 *
 * Copy the name of the <entry>th entry in the directory <dir> into <buffer>.
 * Returns zero on success, nonzero on error.
 */

int lfs_list(struct fs_obj *dir, int entry, char *buffer, size_t size) {
	struct fs_obj *daughter;

	if (!dir) {
		return 1;
	}

	daughter = dir->daughter;

	while (daughter) {
		if (entry <= 0) {
			break;
		}
		else {
			daughter = daughter->sister1;
			entry--;
		}
	}

	if (daughter) {
		strlcpy(buffer, daughter->name, size);
		return 0;
	}
	else {
		return 1;
	}
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

	if (!(dir && obj && name)) {
		return 1;
	}

	mutex_spin(&dir->mutex);

	obj->name     = strdup(name);
	obj->mutex    = 0;
	obj->mother   = dir;
	obj->sister0  = NULL;
	obj->sister1  = dir->daughter;
	obj->daughter = NULL;
	dir->daughter = obj;

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

/*****************************************************************************
 * lfs_add
 *
 * Adds the filesystem object <obj> to the local filesystem at path <path>.
 * This function is intended for internal use by drivers, esp. during init.
 */

void lfs_add(struct fs_obj *obj, const char *path) {
	char *path1;
	uint64_t dirfd;
	struct fs_obj *dir;
	
	if (!obj) {
		return;
	}

	path1 = path_parent(path);
	dirfd = lfs_find(0, path1);
	dir = lfs_lookup(dirfd & 0xFFFFFFFF);
	free(path1);

	if (!dir) {
		return;
	}

	path1 = path_name(path);
	lfs_push(dir, obj, path1);
	free(path1);
}
