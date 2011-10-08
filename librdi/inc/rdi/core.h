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
#include <robject.h>

/*****************************************************************************
 * Rhombus Driver Interface (RDI)
 *
 * RDI is a framework built using robjects that provides support to drivers
 * for a variety of tasks, including permissions, VFS management, and I/O.
 * It is composed of a set of core robject classes, which are modified by the
 * driver to incorporate driver code.
 */

void rdi_init();

/*****************************************************************************
 * rdi_class_core (extends robject_class_basic) - driver
 *
 * All RDI objects inherit from this same base class, which contains
 * implementations of the following calls from the driver:
 *
 * Calls:
 *
 * find <path> - R
 *
 *   Attempt to locate the robject with the given path relative to this one.
 *
 *   Default behavior is to return a pointer to the called robject. This is
 *   overridden by most subclasses, however.
 *
 *   Return: "%r" <pointer to robject>
 *
 * cons <type> (additional args based on type) - none
 *   
 *   Requests the creation of a new robject of the given type. 
 *
 *   Default behavior is to construct the new robject unconditionally. This is
 *   not at all secure.
 *
 *   Return: a robject pointer to the new robject
 * 
 * get-access <user> - R
 *
 *   Requests the access bitmap of the robject for a given user ID. This 
 *   "bitmap" is actually a hexadecimal string value. See natio.h for details 
 *   on its contents.
 *
 *   Default behavior is to return the contents of data field 
 *   "access-%X" <user>, if it exists, and otherwise to return 0.
 *
 *   Return: an access bitmap (hexadecimal string) valid for the given user.
 *
 * set-access <user> <bitmap> - A
 *
 *   Requests the the access bitmap of the robject for a given user ID be
 *   changed.
 *
 *   Default behavior is to allow the request only if the source user has
 *   PERM_ALTER granted to them for this robject.
 *
 * sync - W
 *
 *   Requests that the robject be in some way synchronized. This is open for
 *   per-driver interpretation, but tends to imply a sync to disk if 
 *   applicable.
 *
 *   Default behavior is a no-op.
 *
 * Fields:
 *
 * access-%X <uid>
 *
 *   See calls get-access and set-access for details.
 *
 *   Type: uint32_t
 */

extern struct robject *rdi_class_core;
void __rdi_class_core_setup();

struct robject *rdi_core_cons(uint32_t index, uint32_t access);
void            rdi_core_free(struct robject *r);

/******************************************************************************
 * rdi_class_event (extends rdi_class_core) - event
 *
 * Calls:
 *
 * subscribe (target) - R
 *
 * unsubscribe (target) - W
 */

extern struct robject *rdi_class_event;
void __rdi_class_event_setup();

struct robject *rdi_event_cons(uint32_t index, uint32_t access);
void            rdi_event_free(struct robject *r);

/*
 * Old stuff past here
 */

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

extern struct resource *(*__rdi_callback_cons)(uint64_t src, int type);
extern int (*__rdi_callback_open) (uint64_t src, struct resource *obj);
extern int (*__rdi_callback_close)(uint64_t src, struct resource *obj);

/* Access callbacks */
void rdi_set_aclsync(void (*_aclsync)(struct resource *obj));

extern void (*_rdi_callback_aclsync)(struct resource *obj);

/*****************************************************************************
 * RDI core handlers
 */

void rdi_init_core(void);
void rdi_init_all(void);

char *__rdi_cons_handler (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_open_handler (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_close_handler(uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_type_handler (uint64_t src, uint32_t idx, int argc, char **argv);
char *__rdi_size_handler (uint64_t src, uint32_t idx, int argc, char **argv);

#endif/*_RDI_CORE_H*/
