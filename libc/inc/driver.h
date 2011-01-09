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

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <natio.h>
#include <ipc.h>

/* access control lists *****************************************************/

#define ACL_READ	0x01
#define ACL_WRITE	0x02
#define ACL_ALTER	0x04

struct fs_acl {
	struct fs_acl *next;

	uint32_t user;
	uint8_t permit;
};

uint8_t        acl_get(struct fs_acl *acl, uint32_t user);
struct fs_acl *acl_set(struct fs_acl *acl, uint32_t user, uint8_t permit);

uint8_t        acl_get_default(struct fs_acl *acl);
struct fs_acl *acl_set_default(struct fs_acl *acl, uint8_t permit);

void acl_free(struct fs_acl *acl);

/* local filesystem interface ***********************************************/

struct fs_obj {
	int type;
	bool mutex;

	/* file information */
	uint64_t size;
	uint8_t *data;

	/* inode lookup table */
	uint32_t inode;
	struct fs_obj *next;
	struct fs_obj *prev;

	/* directory structure */
	char *name;
	struct fs_obj *mother;
	struct fs_obj *sister0;
	struct fs_obj *sister1;
	struct fs_obj *daughter;

	/* permissions */
	struct fs_acl *acl;

	/* link information */
	uint64_t link;
};

struct fs_obj *lfs_lookup(uint32_t inode);

void     lfs_root(struct fs_obj *root);
uint64_t lfs_find(uint32_t inode, const char *path, bool nolink);

int lfs_list(struct fs_obj *dir, int entry, char *buffer, size_t size);
int lfs_push(struct fs_obj *dir, struct fs_obj *obj, const char *name);
int lfs_pull(struct fs_obj *obj);

void lfs_add(struct fs_obj *obj, const char *path);

/* wrapper function prototypes **********************************************/

void lfs_wrapper  (struct msg *msg);
void read_wrapper (struct msg *msg);
void write_wrapper(struct msg *msg);
void sync_wrapper (struct msg *msg);
void reset_wrapper(struct msg *msg);
void mmap_wrapper (struct msg *msg);

void find_wrapper(struct mp_fs *cmd);
void lfnd_wrapper(struct mp_fs *cmd);
void cons_wrapper(struct mp_fs *cmd);
void move_wrapper(struct mp_fs *cmd);
void remv_wrapper(struct mp_fs *cmd);
void link_wrapper(struct mp_fs *cmd);
void list_wrapper(struct mp_fs *cmd);
void size_wrapper(struct mp_fs *cmd);
void type_wrapper(struct mp_fs *cmd);
void perm_wrapper(struct mp_fs *cmd);
void auth_wrapper(struct mp_fs *cmd);

/* driver interface structure ***********************************************/

extern struct driver {

	/* initialization */
	void (*init)(int argc, char **argv);

	/* filesystem operations */
	struct fs_obj *(*cons)(int type);
	int            (*push)(struct fs_obj *obj);
	int            (*pull)(struct fs_obj *obj);
	int            (*free)(struct fs_obj *obj);

	/* file operations */
	uint64_t (*size) (struct fs_obj *file);
	size_t   (*read) (struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset);
	size_t   (*write)(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset);
	int      (*reset)(struct fs_obj *file);
	int      (*sync) (struct fs_obj *file);
	void *   (*mmap) (struct fs_obj *file, size_t size, uint64_t offset, int prot);

	/* irq handler */
	void (*irq)(void);
	
} *active_driver;

void driver_init(struct driver *driver, int argc, char **argv);

/* port access **************************************************************/

uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/* IRQ redirection **********************************************************/

void rirq(uint8_t irq);
void irq_wrapper(struct msg *msg);

#endif/*DRIVER_H*/
