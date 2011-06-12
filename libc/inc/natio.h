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

#ifndef NATIO_H
#define NATIO_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <path.h>
#include <ipc.h>

/*****************************************************************************
 * resource pointers
 *
 * This stuff right here, this is important. Resource pointers are the way
 * that Rhombus indentifies pretty much everything on the system. Every file
 * can be uniquely identified by a resource pointer, which, because it is an 
 * integer type, is easy to pass to and from functions. Resource pointers 
 * also have canonical string representations which are used in the rcall and 
 * event interfaces, and are human-readable.
 *
 * The string representations have the following format:
 *
 *   @p:i
 *
 * Where p is the PID of the pointer and i is the index, both in decimal.
 */

#define RP_CONS(pid, idx) ((((uint64_t) (pid)) << 32) | (uint64_t) (idx))
#define RP_PID(rp) ((uint32_t) ((rp) >> 32))
#define RP_INDEX(rp) ((uint32_t) ((rp) & 0xFFFFFFFF))
#define RP_NULL ((uint64_t) 0)

// string represenation (also %r in printf/scanf)
char    *rtoa(uint64_t rp);
uint64_t ator(const char *str);

/* core I/O *****************************************************************/

size_t read (uint64_t rp, void *buf, size_t size, uint64_t offset);
size_t write(uint64_t rp, void *buf, size_t size, uint64_t offset);
int    sync (uint64_t rp);
int    reset(uint64_t rp);
int    share(uint64_t rp, void *buf, size_t size, uint64_t offset, int prot);

/* rcall ********************************************************************/

typedef char *(*rcall_t)(uint64_t src, uint32_t index, int argc, char **argv);

char   *rcall    (uint64_t rp, const char *args);
char   *rcallf   (uint64_t rp, const char *fmt, ...);

int     rcall_set(const char *call, rcall_t handler);
rcall_t rcall_get(const char *call);

/* event ********************************************************************/

struct event_list {
	uint64_t target;
	struct event_list *next;
	struct event_list *prev;
};

struct event_list *event_list_add(struct event_list *list, uint64_t target);
struct event_list *event_list_del(struct event_list *list, uint64_t target);

int    event (uint64_t rp, uint64_t value);
int    eventl(struct event_list *list, uint64_t value);

typedef void (*event_handler_t)(uint64_t source, uint64_t value);
int    event_register(uint64_t source, event_handler_t handler);

/*****************************************************************************
 * resource types
 *
 * All resources must have exactly one of the following type flags:
 *   FS_TYPE_FILE					(f)
 *   FS_TYPE_DIR					(d)
 *
 * Files of may have zero or one of these type flag sets:
 *   FS_TYPE_GRAPH 					(g)
 *   FS_TYPE_CHAR  					(c)
 *   FS_TYPE_LINK 					(l)
 *   FS_TYPE_EVENT 					(e)
 *   FS_TYPE_EVENT | FS_TYPE_GRAPH	(w)
 *
 * Directories may not have any additional type flags.
 */

int fs_type(const char *path);
int rp_type(uint64_t rp);

char typechar(int type);
int  typeflag(char type);

#define FS_TYPE_FILE	0x01	// file (allows read, write, reset)
#define FS_TYPE_DIR		0x02	// directory (allows find, link, list, etc.)
#define FS_TYPE_LINK	0x04	// symbolic link
#define FS_TYPE_PLINK	0x08	// pointer link (similar to mountpoint)
#define FS_TYPE_EVENT	0x10	// event source (allows register, deregister)
#define FS_TYPE_GRAPH	0x20	// graphics file (allows various)
#define FS_TYPE_CHAR	0x40	// character device (disallows size, offsets)

#define FS_IS_FILE(t) (((t) & FS_TYPE_FILE) != 0)
#define FS_IS_DIR(t) ((t) == FS_TYPE_DIR)

/* filesystem operations ****************************************************/

int      io_link(const char *name, uint64_t rp);

extern uint64_t fs_root;

uint64_t fs_find (const char *path);
uint64_t fs_lfind(const char *path);
uint64_t fs_cons (const char *path, int type);
int      fs_remv (const char *path);
char    *fs_list (const char *path, int entry);
int      fs_slnk (const char *path, const char *link);
int      fs_link (const char *path, const char *link);
int      fs_plink(uint64_t link, uint64_t fobj);
uint64_t fs_size (const char *path);

uint64_t rp_size (uint64_t rp);
int      rp_remv (uint64_t rp);

/*****************************************************************************
 * lock types
 *
 * There are five different basic types of resource locks, each providing a
 * different type of permission and/or exclusion. All locks are mandatory in
 * nature, meaning that the relevant operations can only be performed if a
 * lock is granted.
 *
 * LOCK_RS - Read-Shared
 *
 * This is the default lock type (all files act as if LOCK_RS was aquired by
 * every process, unless LOCK_PX is aquired). It allows reading but not 
 * writing. It may be aquired at any point unless LOCK_PX is aquired. It does
 * not prevent any locks from being aquired, even LOCK_PX. It does not 
 * guarantee consistency of reads.
 *
 * LOCK_RX - Read-eXclusive
 *
 * This lock allows reading but not writing. It may be aquired unless LOCK_WS,
 * LOCK_WX, or LOCK_PX are aquired. It guarantees consistency of reads. This 
 * is similar to the shared lock used with flock().
 *
 * LOCK_WS - Write-Shared
 *
 * This lock allows reading and writing. It may be aquired unless LOCK_WX,
 * LOCK_RX, or LOCK_PX are aquired. It does not guarantee consistency of 
 * reads or writes.
 *
 * LOCK_WX - Write-eXclusive
 *
 * This lock allows reading and writing. It may be aquired unless LOCK_RX,
 * LOCK_WS, LOCK_WX, or LOCK_PX are aquired. It guarantees consistency of 
 * reads and writes. This is similar to the exclusive lock used with flock().
 *
 * LOCK_PX - Private-eXclusive
 *
 * This lock allows reading and writing. It may be aquired unless LOCK_RX,
 * LOCK_WS, LOCK_WX, or LOCK_PX are aquired. It prevents LOCK_RS from being
 * aquired as well, and invalidates all LOCK_RS locks while it is aquired.
 * It guarantees consistency of reads and writes as well as privacy of file 
 * contents. It is used for files that have "process owners", such as windows 
 * and nameless pipes.
 */

int fs_setlock(uint64_t rp, int locktype);
int fs_getlock(uint64_t rp);

#define LOCK_NO		0x00
#define LOCK_RS		0x01
#define LOCK_RX		0x02
#define LOCK_WS		0x03
#define LOCK_WX		0x04
#define LOCK_PX		0x05

/*****************************************************************************
 * permission bitmap
 *
 * The following flags correspond to bits that may be set in the permission
 * bitmap. Permissions can be assigned on a per-user basis, but not a 
 * per-process basis.
 *
 * PERM_READ
 *
 * This flag obstensibly gives read access. More correctly, it allows a user 
 * to aquire read locks (RS, RX) on a file. For directories, reading means 
 * listing contents and finding paths.
 *
 * PERM_WRITE
 *
 * This flag obstensibly gives write access. More correctly, it allows a user
 * to aquire write locks (WS, WX, PX) on a file. For directories, writing
 * means creating directory entries.
 *
 * PERM_ALTER
 *
 * This flag allows the permission bitmap to be modified. Some drivers simply
 * do not allow certain operations (usually writing, if the filesystem is
 * read-only) and this does not ensure that the permission bitmap will 
 * actually be modified as specified.
 *
 * PERM_XLOCK
 *
 * This flag allows a user to aquire exclusive locks (RX, WX, PX) on a file.
 */

uint8_t fs_getperm(const char *path, uint32_t user);
int     fs_setperm(const char *path, uint32_t user, uint8_t perm);

uint8_t rp_getperm(uint64_t rp, uint32_t user);
int     rp_setperm(uint64_t rp, uint32_t user, uint8_t perm);

#define PERM_READ	0x01
#define PERM_WRITE	0x02
#define PERM_ALTER	0x04
#define PERM_XLOCK	0x08

#endif/*NATIO_H*/
