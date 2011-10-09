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

#ifndef _RDI_VFS_H
#define _RDI_VFS_H

#include <rdi/util.h>
#include <rdi/core.h>
#include <natio.h>

/*****************************************************************************
 * rdi_class_dir (extends rdi_class_core) - dir
 *
 * Calls:
 *
 * find <path> - R
 *
 *   Attempt to locate the robject with the given path in the directory tree.
 *
 *   Default behavior is complex.
 *
 *   Return: one of two things, depending on whether a symlink is hit:
 *     symlink hit: ">> %s/%s" <link contents> <remaining path>
 *     no symlink: "%r" <pointer to robject>
 *
 * list - R
 *
 *   List the contents of the directory.
 *
 *   Default behavior is to concatenate all of the directory entry names
 *   separated by spaces, escaping spaces within individual entries.
 *
 *   Return: directory contents, space separated.
 *
 * link <entry> <index> - W
 *
 *   Add a new directory entry, linking to the robject with the given index.
 *
 *   Default behavior is to add the new directory entry iff the index refers
 *   to a valid robject and the directory has write access.
 *
 * unlink <entry> - W
 *
 *   Remove the given directory entry.
 *
 *   Default behavior is to remove the directory entry iff the directory has
 *   write access.
 *
 * Fields:
 *
 * dirent-%s <entry>
 *
 *   Directory entry (i.e. hard link) corresponding to the embedded entry
 *   name.
 *
 *   Type: struct robject * (extending rdi_class_core)
 */

extern struct robject *rdi_class_dir;
void __rdi_class_dir_setup();

struct robject *rdi_dir_cons(uint32_t index, uint32_t access);
void            rdi_dir_free(struct robject *r);

void rdi_vfs_add(struct robject *dir, const char *path, struct robject *r);

/*****************************************************************************
 * rdi_class_link (extends rdi_class_core) - link
 *
 * Calls:
 *
 * find <path> - R
 *
 *   Return: ">> %s/%s" <link contents> <path>
 *
 * get-link - R
 *
 *   Return: link redirect.
 *
 * set-link <link> - W
 *
 *   Set the link redirect to the given string value.
 *
 * Fields:
 *
 * link
 *
 *   Symbolic link redirect.
 *   
 *   Type: char * (heap-allocated)
 */

extern struct robject *rdi_class_link;
void __rdi_class_link_setup();

struct robject *rdi_link_cons(uint32_t index, uint32_t access, const char *link);
void            rdi_link_free(struct robject *r);

/*
 * Old stuff past here
 */

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

struct resource *vfs_find  (struct vfs_node *root, const char *path, const char **tail);
char            *vfs_list  (struct vfs_node *dir);
int              vfs_link  (struct vfs_node *dir, const char *name, struct resource *r);
int              vfs_unlink(struct vfs_node *dir, const char *name);

int vfs_add(struct resource *root, const char *path, struct resource *obj);

/*****************************************************************************
 * RDI VFS driver callbacks
 */

void rdi_set_dirsync(void (*_dirsync)(struct resource *obj));
void rdi_set_lnksync(void (*_dirsync)(struct resource *obj));

extern void (*__rdi_callback_dirsync)(struct resource *obj);
extern void (*__rdi_callback_lnksync)(struct resource *obj);

/*****************************************************************************
 * RDI VFS handlers
 */

void rdi_init_vfs(void);
int vfs_init(void);

char *__rdi_find_handler   (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_link_handler   (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_list_handler   (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_symlink_handler(uint64_t src, uint32_t idx, int argc, char **argv);

#endif/*_RDI_VFS_H*/
