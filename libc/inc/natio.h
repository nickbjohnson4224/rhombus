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

#ifndef NATIO_H
#define NATIO_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ipc.h>

/* native I/O routines *****************************************************/

size_t ssend(FILE *file, void *r, void *s, size_t size, uint64_t off, uint8_t port);
size_t read (FILE *file, void *buf, size_t size, uint64_t offset);
size_t write(FILE *file, void *buf, size_t size, uint64_t offset);

/* virtual filesystem operations *******************************************/

#define MAX_PATH	1000

extern FILE *vfs_root;

#define VFS_VERB	0x00F0
#define VFS_NEW 	0x0010	// Create a new VFS object
#define VFS_DEL 	0x0020	// Delete a VFS object
#define VFS_MOV 	0x0030	// Move a VFS object around
#define VFS_GET 	0x0040	// Get a VFS object field
#define VFS_SET 	0x0050	// Set a VFS object field

#define VFS_ACT		0x0000	// Perform a VFS action
#define VFS_ERR		0x1000	// Error from a VFS action

#define VFS_NOUN	0x000F
#define VFS_TYPE	0x0000	// Type of VFS object
#define VFS_FILE	0x0001	// File location of VFS object
#define VFS_DIR 	0x0002	// Directory VFS object / Directory contents
#define VFS_LINK	0x0003	// Link VFS object / Link location
#define VFS_PERM	0x0004	// Permission of VFS object
#define VFS_USER	0x0005	// Owner of VFS object
#define VFS_SIZE	0x0006	// Size of a VFS object
#define VFS_PATH	0x0007	// Path of a VFS object (used for errors)

#define PERM_NEW	0x01	// Permission to create new files in a directory
#define PERM_DEL	0x02	// Permission to delete a file
#define PERM_GET	0x04	// Permission to get all metadata about a file
#define PERM_PERM	0x08	// Permission to set the permissions of a file
#define PERM_LINK	0x10	// Permission to modify a link target
#define PERM_READ	0x20	// Permission to read from a file
#define PERM_WRITE	0x40	// Permission to write to a file

struct vfs_query {
	uint32_t opcode;
	uint32_t file0[2];
	uint32_t file1[2];
	uint32_t value0;
	uint32_t value1;
	char path0[MAX_PATH];
	char path1[MAX_PATH];
} __attribute__ ((packed));

struct vfs_query *vfssend(FILE *root, struct vfs_query *query);

FILE *vfs_new_file(FILE *root, const char *path);
FILE *vfs_new_dir (FILE *root, const char *path);
FILE *vfs_new_link(FILE *root, const char *path, const char *link, FILE *alink);

int vfs_del_file(FILE *root, const char *path);

int vfs_mov_file(FILE *root, const char *path0, const char *path1);

uint16_t vfs_get_type(FILE *root, const char *path);
FILE    *vfs_get_file(FILE *root, const char *path);
char    *vfs_get_list(FILE *root, const char *path);
char    *vfs_get_link(FILE *root, const char *path);
FILE    *vfs_get_alnk(FILE *root, const char *path);
uint8_t  vfs_get_perm(FILE *root, const char *path, uint32_t user);
uint64_t vfs_get_size(FILE *root, const char *path);

int vfs_set_link(FILE *root, const char *path, const char *link, FILE *alink);
int vfs_set_perm(FILE *root, const char *path, uint32_t user, uint8_t perm);
int vfs_set_user(FILE *root, const char *path, uint32_t user);

/* local filesystem operations (for drivers) *******************************/

struct lfs_node {

	/* file information */
	uint64_t size;

	/* file location */
	uint32_t inode;
	struct lfs_node *next;
	struct lfs_node *prev;

	/* directory structure */
	uint16_t type;
	const char *name;
	struct lfs_node *mother;
	struct lfs_node *sister0;
	struct lfs_node *sister1;
	struct lfs_node *daughter;

	/* link location */
	const char *link;
	const FILE *alink;

	/* permissions */
	uint32_t user;
	uint8_t perm_user;
	uint8_t perm_def;
};

void lfs_event(struct packet *packet, uint8_t port, uint32_t caller);
void lfs_event_start(void);
void lfs_event_stop(void);

typedef void (*lfs_handler_t)(struct vfs_query *query, uint32_t inode, uint32_t caller);
void lfs_when_new(lfs_handler_t handler);
void lfs_when_del(lfs_handler_t handler);
void lfs_when_mov(lfs_handler_t handler);
void lfs_when_get(lfs_handler_t handler);
void lfs_when_set(lfs_handler_t handler);

void lfs_get_default(struct vfs_query *query, uint32_t inode, uint32_t caller);
void lfs_set_default(struct vfs_query *query, uint32_t inode, uint32_t caller);

struct lfs_node *lfs_new_file(uint32_t inode, uint64_t size);
struct lfs_node *lfs_new_dir (uint32_t inode);
struct lfs_node *lfs_new_link(const char *link, const FILE *alink);

size_t lfs_list_dir(char *buffer, size_t size, struct lfs_node *dir);

uint32_t lfs_add(struct lfs_node *node, const char *path);
uint32_t lfs_del(const char *path);

uint8_t lfs_get_perm(struct lfs_node *node, uint32_t user);
int     lfs_set_perm(struct lfs_node *node, uint32_t user, uint8_t perm);

uint32_t         lfs_add_path(struct lfs_node *root, const char *path, struct lfs_node *node);
struct lfs_node *lfs_get_path(struct lfs_node *root, const char *path);
struct lfs_node *lfs_get_link(struct lfs_node *root, const char *path, const char **tail);
struct lfs_node *lfs_del_path(struct lfs_node *root, const char *path);

uint32_t         lfs_add_node(struct lfs_node *node);
struct lfs_node *lfs_get_node(uint32_t inode);
struct lfs_node *lfs_del_node(uint32_t inode);

/* path manipulation *******************************************************/

#define PATH_SEP '/'

struct path {
	const char *str;
	const char *pos;
};

struct path *path_cons(const char *path);

char *path_next(struct path *path);
char *path_peek(struct path *path);
int   path_prev(struct path *path);

const char *path_tail(struct path *path);

#endif/*NATIO_H*/
