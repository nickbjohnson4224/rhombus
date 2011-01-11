/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

/* resource pointer macros *************************************************/

#define RP_CONS(pid, idx) ((((uint64_t) (pid)) << 32) | (uint64_t) (idx))
#define RP_PID(rp) ((rp) >> 32)
#define RP_INDEX(rp) ((rp) & 0xFFFFFFFF)
#define RP_NULL ((uint64_t) 0)

/* high level message primitives *******************************************/

struct mp_basic {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
};

#define MP_PROT_BASIC 0 // vanilla message protocol (gcd of all others)
#define MP_PROT_ERROR 1 // simple error protocol
#define MP_PROT_IO    2 // I/O message protocol
#define MP_PROT_FS    3 // filesystem message protocol
#define MP_PROT_EVENT 4 // event protocol
#define MP_PROT_RPC   5 // remote procedure call message protocol
// note: all user defined protocols must be > 1024

#define MP_ARCH_L32	0  // little endian 32-bit architecture
#define MP_ARCH_B32	1  // big endian 32-bit architecture
#define MP_ARCH_L64	2  // little endian 64-bit architecture
#define MP_ARCH_B64	3  // big endian 64-bit architecture
#define MP_ARCH_NATIVE MP_ARCH_L32

#define MP_TYPE_ASYNC 0 // asynchronous request
#define MP_TYPE_SYNC  1 // synchronous request
#define MP_TYPE_TSYNC 2 // timeout synchronous request

struct msg *rp_send (uint64_t rp, uint8_t port, struct mp_basic *msg);
struct msg *rp_tsend(uint64_t rp, uint8_t port, struct mp_basic *msg, uint32_t timeout);
int         rp_asend(uint64_t rp, uint8_t port, struct mp_basic *msg);

struct mp_basic *mp_recv(struct msg *msg);

/* simple error protocol ***************************************************/

struct mp_error {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
	int      value;
};

struct mp_error *error_cons(int value);
struct mp_error *error_recv(struct msg *msg);

void error_reply(struct msg *msg, int value);

/* native I/O routines *****************************************************/

struct mp_io {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
	size_t   size;
	uint64_t offset;
	uint8_t  data[];
};

size_t io_send(uint64_t rp, void *r, void *s, size_t size, uint64_t off, uint8_t port);
struct mp_io *io_recv(struct msg *msg);

size_t   read (uint64_t rp, void *buf, size_t size, uint64_t offset);
size_t   write(uint64_t rp, void *buf, size_t size, uint64_t offset);
int      sync (uint64_t rp);
int      reset(uint64_t rp);

uint64_t io_find(const char *name);
uint64_t io_cons(const char *name, int type);
int      io_remv(const char *name);
int      io_link(const char *name, uint64_t rp);

/* filesystem operations ***************************************************/

struct mp_fs {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
	uint64_t v0;
	uint64_t v1;
	uint8_t  op;
	char     s0[4000];
	char     null0;
};

extern uint64_t fs_root;

#define FS_ERR  0x00
#define FS_FIND 0x01
#define FS_CONS 0x02
#define FS_MOVE 0x03
#define FS_REMV 0x04
#define FS_LINK 0x05
#define FS_LIST 0x06
#define FS_SIZE 0x07
#define FS_TYPE 0x08
#define FS_LFND	0x09
#define FS_PERM 0x0A
#define FS_AUTH	0x0B

#define FS_PERM_READ  0x01
#define FS_PERM_WRITE 0x02
#define FS_PERM_ALTER 0x04

#define ERR_NULL 0x00
#define ERR_FILE 0x01
#define ERR_DENY 0x02
#define ERR_FUNC 0x03
#define ERR_TYPE 0x04
#define ERR_FULL 0x05

struct mp_fs *fs_send(uint64_t root, struct mp_fs *cmd);
struct mp_fs *fs_recv(struct msg *msg);

uint64_t fs_find  (uint64_t root, const char *path);
uint64_t fs_cons  (uint64_t dir, const char *name, int type);
uint64_t fs_move  (uint64_t dir, const char *name, uint64_t file);
char    *fs_list  (uint64_t dir, int entry);
int      fs_remove(uint64_t fobj);
int      fs_link  (uint64_t link, uint64_t fobj);
uint64_t fs_size  (uint64_t file);
int      fs_type  (uint64_t fobj);
uint64_t fs_lfind (uint64_t root, const char *path);
uint8_t  fs_perm  (uint64_t fobj, uint32_t user);
int      fs_auth  (uint64_t fobj, uint32_t user, uint8_t perm);

#define FOBJ_NULL	0x00
#define FOBJ_FILE	0x01
#define FOBJ_DIR	0x02
#define FOBJ_PORT	0x04

/* high level events *******************************************************/

struct mp_event {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
	uint32_t event_id;
	uint32_t value;
	uint64_t timestamp;
};

int event_send(uint64_t rp, uint32_t event_id, uint32_t value);
struct mp_event *event_recv(struct msg *msg);

/* remote procedure calling ************************************************/

struct mp_rpc {
	uint32_t length;
	uint32_t index;
	uint16_t protocol;
	uint8_t  arch;
	uint8_t  type;
	uint16_t argc;
	char     args[];
};

int rpc_send (uint64_t rp, char *args);
int rpc_sendv(uint64_t rp, ...);
struct mp_rpc *rpc_recv(struct msg *msg);

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

char *path_parent(const char *path);
char *path_name  (const char *path);

char *path_simplify(const char *path);

/* file descriptor persisence ***********************************************/

uint64_t fdload(int id);
int      fdsave(int id, uint64_t fd);

#endif/*NATIO_H*/
