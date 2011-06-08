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

/* virtual filesystem interface *********************************************/

struct vfs_obj {
	int type;
	bool mutex;

	/* file information */
	uint64_t size;
	uint8_t *data;

	/* index lookup table */
	uint32_t index;
	struct vfs_obj *next;
	struct vfs_obj *prev;

	/* directory structure */
	struct vfs_node *vfs;

	/* permissions */
	struct vfs_acl *acl;

	/* link information */
	uint64_t link;
};

struct vfs_obj *vfs_get(uint32_t index);
struct vfs_obj *vfs_set(uint32_t index, struct vfs_obj *obj);

struct vfs_node {
	bool mutex;

	// associated resource (i.e. hard link)
	struct vfs_obj *resource;

	char *name;
	struct vfs_node *mother;
	struct vfs_node *sister0;
	struct vfs_node *sister1;
	struct vfs_node *daughter;
};

uint64_t vfs_find(struct vfs_obj *root, const char *path, bool nolink);
int      vfs_add (struct vfs_obj *root, const char *path, struct vfs_obj *obj);

struct vfs_obj *vfs_getlink(struct vfs_node *root, const char *path);
int      vfs_setlink(struct vfs_node *root, const char *path, struct vfs_obj *obj);

char *vfs_list(struct vfs_obj *dir, int entry);
int   vfs_push(uint64_t source, struct vfs_obj *dir, struct vfs_obj *obj);
int   vfs_pull(uint64_t source, struct vfs_obj *obj);

/* virtual filesystem request wrapper ***************************************/

int vfs_set_cons(struct vfs_obj *(*vfs_cons)(uint64_t source, int type));
int vfs_set_push(int (*vfs_push)(uint64_t source, struct vfs_obj *obj));
int vfs_set_pull(int (*vfs_pull)(uint64_t source, struct vfs_obj *obj));
int vfs_set_free(int (*vfs_free)(uint64_t source, struct vfs_obj *obj));

int vfs_init(void);

void __cons_wrapper(struct msg *msg);
void __move_wrapper(struct msg *msg);
void __remv_wrapper(struct msg *msg);
void __link_wrapper(struct msg *msg);
void __list_wrapper(struct msg *msg);
void __size_wrapper(struct msg *msg);
void __type_wrapper(struct msg *msg);
void __perm_wrapper(struct msg *msg);
void __auth_wrapper(struct msg *msg);

char *__find_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__cons_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__move_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__remv_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__link_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__list_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__size_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__type_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__perm_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);
char *__auth_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv);

extern struct vfs_obj *(*_vfs_cons)(uint64_t source, int type);
extern int             (*_vfs_push)(uint64_t source, struct vfs_obj *obj);
extern int             (*_vfs_pull)(uint64_t source, struct vfs_obj *obj);
extern int             (*_vfs_free)(uint64_t source, struct vfs_obj *obj);

#endif/*VFS_H*/
