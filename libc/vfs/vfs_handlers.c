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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>
#include <vfs.h>

static char *_find_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_cons_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_link_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_list_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_size_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_type_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_symlink_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_getperm_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_setperm_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_lock_handler(uint64_t source, uint32_t index, int argc, char **argv);

/*****************************************************************************
 * vfs_init
 *
 * Initialize VFS request handling system. Returns zero on success, nonzero on
 * error.
 */

int vfs_init(void) {
	rcall_set("fs_find", _find_handler);
	rcall_set("fs_cons", _cons_handler);
	rcall_set("fs_list", _list_handler);
	rcall_set("fs_link", _link_handler);
	rcall_set("fs_type", _type_handler);
	rcall_set("fs_size", _size_handler);
	rcall_set("fs_symlink", _symlink_handler);
	rcall_set("fs_getperm", _getperm_handler);
	rcall_set("fs_setperm", _setperm_handler);
	rcall_set("fs_lock", _lock_handler);

	return 0;
}

static char *_find_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *root;
	struct resource *file;
	const char *path;
	const char *tail;
	bool link;

	if (argc <= 1) {
		return NULL;
	}

	// check for follow link flag
	if (argc == 2) {
		link = false;
		path = argv[1];
	}
	else if (!strcmp(argv[1], "-L")) {
		link = true;
		path = argv[2];
	}
	else {
		return NULL;
	}

	// find root node
	root = index_get(index);

	if (!root) {
		return strdup("! nfound");
	}

	// find resource
	if (!root->vfs && (path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))) {
		file = root;
		tail = NULL;
	}
	else {
		file = vfs_find(root->vfs, path, &tail);
	}

	if (!file) {
		return strdup("! nfound");
	}

	if (file->symlink && !(!tail && link)) {
		return saprintf(">> %s/%s", file->symlink, (tail) ? tail : "");
	}
	else {
		return rtoa(RP_CONS(getpid(), file->index));
	}
}

static char *_cons_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *dir, *new;
	const char *name;
	uint64_t rp;
	int type;

	/* check request */
	if (argc < 3) {
		return NULL;
	}

	/* make sure active driver can construct new objects */
	if (!_vfs_cons) {
		return strdup("! nosys");
	}

	name = argv[1];
	type = atoi(argv[2]);

	/* get requested parent directory */
	dir = index_get(index);

	if (!dir) {
		return strdup("! nfound");
	}

	mutex_spin(&dir->mutex);

	/* check permissions */
	if (!vfs_permit(dir, source, PERM_WRITE)) {
		mutex_free(&dir->mutex);
		return strdup("! denied");
	}

	if ((dir->type & FS_TYPE_DIR) == 0) {
		mutex_free(&dir->mutex);
		return strdup("! type");
	}

	/* construct new object */
	new = _vfs_cons(source, type);

	if (!new) {
		mutex_free(&dir->mutex);
		return strdup("! construct");
	}

	/* add new resource to directory */
	mutex_spin(&dir->vfs->mutex);
	if (vfs_link(dir->vfs, name, new)) {
		
		/* failure: free the new object */
		if (_vfs_free) {
			_vfs_free(source, new);
		}
		else {
			resource_free(new);
		}

		rp = 0;
	}
	else {
		rp = RP_CONS(getpid(), new->index);
		
		/* add new resource to index */
		index_set(new->index, new);

		/* synchonize directory metadata */
		if (_vfs_sync) _vfs_sync(source, dir);
	}
	mutex_free(&dir->vfs->mutex);
	mutex_free(&dir->mutex);

	return ((rp) ? rtoa(rp) : strdup("! link"));
}

static char *_link_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *dir;
	struct resource *r;
	const char *name;
	uint64_t link;
	int err;

	if (argc < 3) {
		return NULL;
	}

	/* find directory */
	dir = index_get(index);
	if (!dir) return strdup("! nfound");

	if (!FS_IS_DIR(dir->type) || !dir->vfs) {
		/* dir is not a directory */
		return strdup("! type");
	}

	if (!vfs_permit(dir, source, PERM_WRITE)) {
		/* permission denied */
		return strdup("! denied");
	}

	/* parse arguments */
	name = argv[1];
	link = ator(argv[2]);

	if (link) {
		/* create new link */
		
		if (RP_PID(link) != getpid()) {
			/* link is outside current driver */
			return strdup("! extern");
		}

		/* find resource to be linked */
		r = index_get(RP_INDEX(link));
		if (!r) return strdup("! nfound");

		if (FS_IS_DIR(r->type)) {
			/* cannot link directories */
			return strdup("! type");
		}

		/* create link */
		mutex_spin(&dir->vfs->mutex);
		mutex_spin(&r->mutex);
		err = vfs_link(dir->vfs, name, r);
		mutex_free(&r->mutex);

		if (err) {
			mutex_free(&dir->vfs->mutex);
			return strdup("! link");
		}

		/* synchronize with driver */
		mutex_spin(&dir->mutex);
		if (_vfs_sync) _vfs_sync(source, dir);
		mutex_free(&dir->mutex);
		mutex_free(&dir->vfs->mutex);

		return strdup("T");
	}
	else {

		/* find linked resource */
		r = vfs_find(dir->vfs, name, NULL);
		
		/* delete link */
		mutex_spin(&dir->vfs->mutex);
		err = vfs_unlink(dir->vfs, name);
		mutex_free(&dir->vfs->mutex);

		switch (err) {
		case 0: break;
		case 1: return strdup("! type");
		case 2: return strdup("! nfound");
		case 3: return strdup("! notempty");
		default: return NULL;
		}

		/* if refcount is zero, free resource */
		if (r->vfs_refcount == 0) {
			if (_vfs_free) _vfs_free(source, r);
			else resource_free(r);
		}

		return strdup("T");
	}
}

static char *_symlink_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	char *symlink;

	if (argc < 2) {
		return NULL;
	}

	r = index_get(index);
	if (!r) return strdup("! nfound");

	symlink = argv[1];
	
	if (!FS_IS_LINK(r->type)) {
		return strdup("! type");
	}

	mutex_spin(&r->mutex);

	if (!vfs_permit(r, source, PERM_WRITE)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	if (r->symlink) {
		free(r->symlink);
	}
	
	r->symlink = strdup(symlink);

	mutex_free(&r->mutex);

	return strdup("T");
}

static char *_list_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *dir;
	uint32_t entry;
	char *name;

	if (argc <= 1) {
		return NULL;
	}

	/* find directory */
	dir = index_get(index);

	if (!dir) {
		return strdup("! nfound");
	}

	entry = atoi(argv[1]);

	mutex_spin(&dir->mutex);

	if (!(dir->type & FS_TYPE_DIR)) {
		mutex_free(&dir->mutex);
		return strdup("! notdir");
	}

	if (!vfs_permit(dir, source, PERM_READ)) {
		mutex_free(&dir->mutex);
		return strdup("! denied");
	}

	mutex_spin(&dir->vfs->mutex);
	name = vfs_list(dir->vfs, entry);
	mutex_free(&dir->vfs->mutex);

	if (!name) {
		mutex_free(&dir->mutex);
		return strdup("! nfound");
	}

	mutex_free(&dir->mutex);

	return name;
}

static char *_size_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint64_t size;

	r = index_get(index);
	if (!r) return strdup("! nfound");

	mutex_spin(&r->mutex);
	if (!vfs_permit(r, source, PERM_READ)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	if ((r->type & FS_TYPE_FILE) == 0 || (r->type & FS_TYPE_CHAR) != 0) {
		mutex_free(&r->mutex);
		return strdup("! type");
	}

	size = r->size;
	mutex_free(&r->mutex);

	return saprintf("%d:%d", (uint32_t) (size >> 32), (uint32_t) size);
}

static char *_type_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	char *reply;

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	reply = malloc(sizeof(char) * 2);
	reply[1] = '\0';
	reply[0] = typechar(r->type);

	return reply;
}

static char *_getperm_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint32_t user;
	uint8_t perm;

	if (argc < 2) {
		return NULL;
	}

	user = atoi(argv[1]);
	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);
	perm = id_hash_get(&r->acl, user);
	mutex_free(&r->mutex);

	return saprintf("%d", perm);
}

static char *_setperm_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint32_t user;
	uint8_t perm;

	if (argc < 3) {
		return NULL;
	}

	user = atoi(argv[1]);
	perm = atoi(argv[2]);

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);

	if (!vfs_permit(r, source, PERM_ALTER)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	id_hash_set(&r->acl, user, perm);

	mutex_free(&r->mutex);

	return strdup("T");
}

static char *_lock_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	int locktype;

	if (argc < 2) {
		return NULL;
	}

	locktype = atoi(argv[1]);
	
	r = index_get(index);
	if (!r) return strdup("! nfound");

	mutex_spin(&r->mutex);
	if (!vfs_permit(r, source, PERM_LOCK)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	if (!r->lock) {
		r->lock = vfs_lock_cons();
	}

	if (vfs_lock_acquire(r->lock, RP_PID(source), locktype)) {
		mutex_free(&r->mutex);
		return strdup("! locked");
	}

	mutex_free(&r->mutex);
	return strdup("T");
}
