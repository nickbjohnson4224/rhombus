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

#ifndef DRIVER_H
#define DRIVER_H

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
	char *name;
	struct vfs_obj *mother;
	struct vfs_obj *sister0;
	struct vfs_obj *sister1;
	struct vfs_obj *daughter;

	/* permissions */
	struct vfs_acl *acl;

	/* link information */
	uint64_t link;
};

struct vfs_obj *vfs_get_index(uint32_t index);
struct vfs_obj *vfs_set_index(uint32_t index, struct vfs_obj *obj);

uint64_t vfs_find(struct vfs_obj *root, const char *path, bool nolink);
int      vfs_add (struct vfs_obj *root, const char *path, struct vfs_obj *obj);

int vfs_dir_list(struct vfs_obj *dir, int entry, char *buffer, size_t size);
int vfs_dir_push(uint64_t source, struct vfs_obj *dir, struct vfs_obj *obj, const char *name);
int vfs_dir_pull(uint64_t source, struct vfs_obj *obj);

/* virtual filesystem request wrapper ***************************************/

int vfs_wrap_cons(struct vfs_obj *(*vfs_cons)(uint64_t source, int type));
int vfs_wrap_push(int (*vfs_push)(uint64_t source, struct vfs_obj *obj));
int vfs_wrap_pull(int (*vfs_pull)(uint64_t source, struct vfs_obj *obj));
int vfs_wrap_free(int (*vfs_free)(uint64_t source, struct vfs_obj *obj));

int vfs_wrap_init(void);

void __find_wrapper(struct msg *msg);
void __cons_wrapper(struct msg *msg);
void __move_wrapper(struct msg *msg);
void __remv_wrapper(struct msg *msg);
void __link_wrapper(struct msg *msg);
void __list_wrapper(struct msg *msg);
void __size_wrapper(struct msg *msg);
void __type_wrapper(struct msg *msg);
void __perm_wrapper(struct msg *msg);
void __auth_wrapper(struct msg *msg);

extern struct vfs_obj *(*_vfs_cons)(uint64_t source, int type);
extern int             (*_vfs_push)(uint64_t source, struct vfs_obj *obj);
extern int             (*_vfs_pull)(uint64_t source, struct vfs_obj *obj);
extern int             (*_vfs_free)(uint64_t source, struct vfs_obj *obj);

/* I/O request wrapper ******************************************************/

int di_wrap_read (size_t   (*di_read) (uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_write(size_t   (*di_write)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_reset(int      (*di_reset)(uint64_t source, struct vfs_obj *file));
int di_wrap_sync (int      (*di_sync) (uint64_t source, struct vfs_obj *file));
int di_wrap_size (uint64_t (*di_size) (uint64_t source, struct vfs_obj *file));
int di_wrap_share(int      (*di_share)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));

void __read_wrapper (struct msg *msg);
void __write_wrapper(struct msg *msg);
void __sync_wrapper (struct msg *msg);
void __reset_wrapper(struct msg *msg);
void __share_wrapper(struct msg *msg);

extern size_t   (*_di_read) (uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
extern size_t   (*_di_write)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
extern int      (*_di_reset)(uint64_t source, struct vfs_obj *file);
extern int      (*_di_sync) (uint64_t source, struct vfs_obj *file);
extern uint64_t (*_di_size) (uint64_t source, struct vfs_obj *file);
extern int      (*_di_share)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);

/* port access **************************************************************/

uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/* IRQ redirection **********************************************************/

int di_wrap_irq(uint8_t irq, void (*irq_handler)(struct msg *msg));

/* virtual 8086 mode ********************************************************/

#define VM86_SP	0x7C00
#define VM86_SS	0x0000
#define VM86_IP 0x7C00
#define VM86_CS 0x0000

int __vm86(uint32_t ipcs, uint32_t spss);

// identity maps all of lower memory at 0x00000 through 0x100000
int vm86_setup(void);

// loads code at 0x7C00 with a stack at 0x7C00 (calls vm86_setup if needed)
int vm86_exec (void *code, size_t size);

#endif/*DRIVER_H*/
