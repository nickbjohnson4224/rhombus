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

#ifndef _RDI_CORE_H
#define _RDI_CORE_H

#include <rdi/util.h>

/* RDI resource structure ****************************************************/

struct resource {
	bool mutex;

	/* file information */
	int      type;
	uint64_t size;
	uint8_t *data;

	/* opener PID table */
	struct id_hash open;

	/* index lookup table */
	uint32_t index;
	struct resource *next;
	struct resource *prev;

	/* directory structure */
	struct vfs_node *vfs;
	int vfs_refcount;

	/* old permissions system (deprecated) */
	struct id_hash acl;

	/* access control */
	struct id_hash pid_acl;
	struct id_hash uid_acl;
	uint32_t dfl_acl;

	/* link information */
	char *symlink;
};

struct resource *resource_cons(int type, int access);
void             resource_free(struct resource *r);

/******************************************************************************
 * RDI resource indexing
 *
 * A central part of the RDI is the resource index. It is a table that maps
 * every resource to an index number, which is part of the resource pointer
 * that refers to that resource (along with the driver PID.)
 *
 * Functions:
 *
 *   index_get returns the resource registered with index <index>.
 *
 *   index_set sets the resource <r> to be registered with index <index>, 
 *   setting the index field of <r> to <index> in the process. If another
 *   resource already has index <index>, it is returned.
 *
 *   index_new returns an index number that is unused by any resource in the
 *   index table. This index number has almost always never been used by a
 *   resource, but this behavior is not guaranteed.
 */

struct resource *index_get(uint32_t index);
struct resource *index_set(uint32_t index, struct resource *r);
uint32_t         index_new(void);

/******************************************************************************
 * RDI core driver callbacks
 *
 * Drivers that use RDI register their functionality with RDI by using these
 * functions. The functions set here are global for the driver, but may be
 * overridden by resources individually.
 *
 * When a callback is run, it could be run by any thread at any time. Make sure
 * to use adequate synchonization.
 */

void rdi_set_cons (struct resource *(*_cons)(uint64_t src, int type));
void rdi_set_open (int (*_open) (uint64_t src, struct resource *obj));
void rdi_set_close(int (*_close)(uint64_t src, struct resource *obj));

extern struct resource *(*_rdi_callback_cons)(uint64_t src, int type);
extern int (*_rdi_callback_open) (uint64_t src, struct resource *obj);
extern int (*_rdi_callback_close)(uint64_t src, struct resource *obj);

/* I/O callbacks */
void rdi_set_read (size_t (*_read) (uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_write(size_t (*_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_reset(void   (*_reset)(uint64_t src, uint32_t idx));
void rdi_set_sync (void   (*_sync) (uint64_t src, uint32_t idx));
void rdi_set_share(void   (*_share)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_mmap (void  *(*_mmap) (uint64_t src, uint32_t idx, size_t size, uint64_t off, int prot));

extern size_t (*_rdi_callback_read) (uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
extern size_t (*_rdi_callback_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
extern void   (*_rdi_callback_reset)(uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_sync) (uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_share)(uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_mmap) (uint64_t src, uint32_t idx, size_t size, uint64_t off, int prot);

/* VFS callbacks */
void rdi_set_dirsync(void (*_dirsync)(struct resource *obj));
void rdi_set_lnksync(void (*_lnksync)(struct resource *obj));

extern void (*_rdi_callback_dirsync)(struct resource *obj);
extern void (*_rdi_callback_lnksync)(struct resource *obj);

/* Access callbacks */
void rdi_set_aclsync(void (*_aclsync)(struct resource *obj));

extern void (*_rdi_callback_aclsync)(struct resource *obj);

/*****************************************************************************
 * RDI core handlers
 */

void rdi_init_core();
void rdi_init_all();

char *__rdi_cons_handler (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_open_handler (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_close_handler(uint64_t src, uint32_t idx, int argc, char **argv);

#endif/*_RDI_CORE_H*/
