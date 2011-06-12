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

#ifndef VFS_H
#define VFS_H

#include <natio.h>

/* access control lists *****************************************************/

struct vfs_acl {
	struct vfs_acl *next;

	uint32_t user;
	uint8_t permit;
};

uint8_t         acl_get(struct vfs_acl *acl, uint32_t user);
struct vfs_acl *acl_set(struct vfs_acl *acl, uint32_t user, uint8_t permit);

uint8_t         acl_get_default(struct vfs_acl *acl);
struct vfs_acl *acl_set_default(struct vfs_acl *acl, uint8_t permit);

void acl_free(struct vfs_acl *acl);

/* resource structure *******************************************************/

struct resource {
	int type;
	bool mutex;

	/* file information */
	uint64_t size;
	uint8_t *data;

	/* index lookup table */
	uint32_t index;
	struct resource *next;
	struct resource *prev;

	/* directory structure */
	struct vfs_node *vfs;

	/* permissions */
	struct vfs_acl *acl;

	/* link information */
	uint64_t link;
};

struct resource *index_get(uint32_t index);
struct resource *index_set(uint32_t index, struct resource *r);

/* virtual filesystem interface *********************************************/

struct vfs_node {
	bool mutex;

	// associated resource (i.e. hard link)
	struct resource *resource;

	// name
	char *name;

	// directory structure
	struct vfs_node *mother;
	struct vfs_node *sister0;
	struct vfs_node *sister1;
	struct vfs_node *daughter;
};

int vfs_add(struct resource *root, const char *path, struct resource *obj);
struct resource *vfs_find(struct vfs_node *root, const char *path, const char **tail);

int vfs_link(struct vfs_node *root, const char *path, struct resource *obj);

char *vfs_list(struct resource *dir, int entry);
int   vfs_push(uint64_t source, struct resource *dir, struct resource *obj);
int   vfs_pull(uint64_t source, struct resource *obj);

/* virtual filesystem request wrapper ****************************************
 *
 * The VFS request wrapper contains three functions that may be provided by
 * the driver for use by the VFS. Their behavior is as follows:
 *
 * _vfs_cons (set by vfs_set_free)
 *
 * This function creates a new resource of the given type <type> and returns
 * a pointer to the resource structure on success (NULL on failure). This is
 * used by the VFS when a request for a new file, directory, etc. is recieved.
 *
 * _vfs_sync (set by vfs_set_sync)
 *
 * This function synchronizes the contents of a VFS directory structure
 * with its corresponding directory resource. If there are any differences
 * between the VFS directory structure of the given resource and the
 * directory contents of that resource, the resource's version is modified.
 * This is used by the VFS whenever a directory structure is modified by
 * a request, but not when it is modified internally, i.e. by vfs_add.
 *
 * _vfs_free (set by vfs_set_free)
 *
 * This function destroys the given resource. If this function is not defined
 * by the driver, the VFS instead free()'s the resource and it's directory
 * structure and ACL, which is only appropriate if no other memory is in any
 * way allocated to the resource (which is rare except for directories in 
 * tmpfs)
 */

int vfs_set_cons(struct resource *(*vfs_cons)(uint64_t source, int type));
int vfs_set_sync(int (*vfs_sync)(uint64_t source, struct resource *obj));
int vfs_set_free(int (*vfs_free)(uint64_t source, struct resource *obj));

int vfs_init(void);

void __link_wrapper(struct msg *msg);
void __perm_wrapper(struct msg *msg);
void __auth_wrapper(struct msg *msg);

char *__find_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__cons_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__remv_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__link_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__list_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__size_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__type_rcall_wrapper   (uint64_t source, uint32_t index, int argc, char **argv);
char *__getperm_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__setperm_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);

extern struct resource *(*_vfs_cons)(uint64_t source, int type);
extern int              (*_vfs_sync)(uint64_t source, struct resource *obj);
extern int              (*_vfs_free)(uint64_t source, struct resource *obj);

#endif/*VFS_H*/
