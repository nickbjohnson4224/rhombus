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
static char *_remv_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_link_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_list_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_size_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_type_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_getperm_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_setperm_handler(uint64_t source, uint32_t index, int argc, char **argv);

/*****************************************************************************
 * vfs_init
 *
 * Initialize VFS request handling system. Returns zero on success, nonzero on
 * error.
 */

int vfs_init(void) {
	when(PORT_LINK, __link_wrapper);

	rcall_set("fs_find", _find_handler);
	rcall_set("fs_cons", _cons_handler);
	rcall_set("fs_list", _list_handler);
	rcall_set("fs_remv", _remv_handler);
	rcall_set("fs_type", _type_handler);
	rcall_set("fs_size", _size_handler);
	rcall_set("fs_getperm", _getperm_handler);
	rcall_set("fs_setperm", _setperm_handler);

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

	if (file->link && !(!tail && link)) {
		return saprintf(">> %r%s", file->link, (tail) ? tail : "");
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
	if ((acl_get(dir->acl, getuser(RP_PID(source))) & PERM_WRITE) == 0) {
		mutex_free(&dir->mutex);
		return strdup("! denied");
	}
	else if ((dir->type & FS_TYPE_DIR) == 0) {
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

static char *_remv_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;

	return strdup("! nosys");

	r = index_get(index);

	if (r) {
		mutex_spin(&r->mutex);

		/* check all permissions */
		if ((acl_get(r->acl, gettuser() & PERM_WRITE) == 0)) {
			return strdup("! denied");
		}

		/* check if directory is empty */
		if (r->vfs && r->vfs->daughter) {
			return strdup("! notempty");
		}
		
		/* remove the object from its directory */
		vfs_pull(source, r);

		if (_vfs_free) {
			/* allow the driver to free the object */
			_vfs_free(source, r);
		}
		else {
			/* free the object, assuming data is not allocated */
			resource_free(r);
		}

		return strdup("T");
	}

	return strdup("! nfound");
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

	if ((acl_get(dir->acl, gettuser()) & PERM_READ) == 0) {
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

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	if ((acl_get(r->acl, gettuser()) & PERM_READ) == 0) {
		return strdup("! denied");
	}

	if ((r->type & FS_TYPE_FILE) == 0 || (r->type & FS_TYPE_CHAR) != 0) {
		return strdup("! type");
	}

	return saprintf("%d:%d", (uint32_t) (r->size >> 32), (uint32_t) r->size);
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
	perm = acl_get(r->acl, user);
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

	if ((acl_get(r->acl, gettuser()) & PERM_ALTER) == 0) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	r->acl = acl_set(r->acl, user, perm);

	mutex_free(&r->mutex);

	return strdup("T");
}
