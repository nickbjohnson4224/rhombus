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
 * For example, if the PID is 42 and the index is 123, the string 
 * representation would be "@42:123".
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

uint64_t rp_cons (uint64_t driver, int type);
uint64_t rp_size (uint64_t rp);
int      rp_open (uint64_t rp);
int      rp_close(uint64_t rp);

/*****************************************************************************
 * resource types
 *
 * All resources must have exactly one of the following type flags:
 *   FS_TYPE_FILE					(f)
 *   FS_TYPE_DIR					(d)
 *   FS_TYPE_LINK					(l)
 *
 * Files of may have zero or one of these type flag sets:
 *   FS_TYPE_GRAPH 					(g)
 *   FS_TYPE_CHAR  					(c)
 *   FS_TYPE_EVENT 					(e)
 *   FS_TYPE_EVENT | FS_TYPE_GRAPH	(w)
 *
 * Directories may not have any additional type flags.
 *
 * Links may not have any additional type flags.
 *
 * Therefore, the following type values are valid:
 *   FS_TYPE_FILE    (f)  (FS_TYPE_FILE)
 *   FS_TYPE_DIR     (d)  (FS_TYPE_DIR)
 *   FS_TYPE_LINK    (l)  (FS_TYPE_LINK)
 *   FS_TYPE_GRAPHF  (g)  (FS_TYPE_FILE | FS_TYPE_GRAPH)
 *   FS_TYPE_EVENTF  (e)  (FS_TYPE_FILE | FS_TYPE_EVENT)
 *   FS_TYPE_WINDOW  (w)  (FS_TYPE_FILE | FS_TYPE_GRAPH | FS_TYPE_EVENT)
 *   FS_TYPE_CHARF   (c)  (FS_TYPE_FILE | FS_TYPE_CHAR)
 */

int fs_type(const char *path);
int rp_type(uint64_t rp);

char typechar(int type);
int  typeflag(char type);

#define FS_TYPE_FILE   0x01 // file (allows read, write, reset)
#define FS_TYPE_DIR    0x02 // directory (allows find, link, list, etc.)
#define FS_TYPE_LINK   0x04 // symbolic link
#define FS_TYPE_EVENT  0x10 // event source flag (allows register, deregister)
#define FS_TYPE_GRAPH  0x20 // graphics file flag (allows various)
#define FS_TYPE_CHAR   0x40 // character device flag (disallows size, offsets)

#define FS_TYPE_GRAPHF 0x21 // graphics file
#define FS_TYPE_EVENTF 0x11 // event source
#define FS_TYPE_WINDOW 0x31 // window
#define FS_TYPE_CHARF  0x41 // character file

#define FS_IS_FILE(t) (((t) & FS_TYPE_FILE) != 0)
#define FS_IS_DIR(t) ((t) == FS_TYPE_DIR)
#define FS_IS_LINK(t) ((t) == FS_TYPE_LINK)

/* filesystem operations ****************************************************/

extern uint64_t fs_root;

uint64_t fs_find (const char *path);
uint64_t fs_lfind(const char *path);
uint64_t fs_cons (const char *path, int type);
char    *fs_list (const char *path);
uint64_t fs_size (const char *path);
uint64_t fs_open (const char *path);

char    *rp_list (uint64_t dir);

/*****************************************************************************
 * links
 *
 * There are two different types of links: hard links and symbolic links.
 *
 * Hard links are simply directory entries, and are used for all directories
 * and files. Because of this, hard links are indistinguishable from files.
 * All metadata of a file is shared between its hard links, except for its
 * position in the VFS. Files may have an unlimited number of hard links, but
 * directories may only have one, to prevent a variety of problems. If you
 * want to link directories, use symbolic links. Once a file or directory has
 * no hard links, it is typically freed. Use fs_link to create hard links and
 * fs_ulink to remove them.
 *
 * Symbolic links are redirections to a specified path. Because paths may
 * contain resource pointers, symbolic links may be used like UNIX
 * mountpoints, because they can span devices without those devices having a
 * contiguous VFS. Symbolic links may be invalid as well, because they don't
 * keep track of their targets. Symbolic links may be created and modified
 * using fs_slink (and fs_plink, which is a more flexible form.)
 */

int fs_slink(const char *path, const char *link);
int fs_plink(const char *path, uint64_t link_rp, const char *link_path);
int fs_link (const char *path, const char *link);
int fs_ulink(const char *path);

int rp_slink(uint64_t rp, const char *link);
int rp_plink(uint64_t rp, uint64_t link_rp, const char *link_path);
int rp_link (uint64_t dir, const char *name, uint64_t link);

/*****************************************************************************
 * permission bitmap
 *
 * The following flags correspond to bits that may be set in the permission
 * bitmap. Permissions can be assigned on a per-user basis, but not a 
 * per-process basis.
 *
 * PERM_READ
 *
 * This flag allows read access. For directories, this means finding and 
 * listing. For files, this means reading file contents.
 *
 * PERM_WRITE
 *
 * This flag allows write access. For directories, this means the creation and
 * deletion of hard links. For files, this means writing, clearing, and 
 * deleting files/file contents, as well as requesting file synchronization.
 * 
 * PERM_ALTER
 *
 * This flag allows the permission bitmap to be modified. Some drivers simply
 * do not allow certain operations (usually writing, if the filesystem is
 * read-only) and this does not ensure that the permission bitmap will 
 * actually be modified as specified.
 v

uint8_t fs_getperm(const char *path, uint32_t user);
int     fs_setperm(const char *path, uint32_t user, uint8_t perm);

uint8_t rp_getperm(uint64_t rp, uint32_t user);
int     rp_setperm(uint64_t rp, uint32_t user, uint8_t perm);

#define PERM_READ	0x01
#define PERM_WRITE	0x02
#define PERM_ALTER	0x04

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

int event_register(uint64_t rp);
int event_deregister(uint64_t rp);

int event (uint64_t rp, const char *value);
int eventl(struct event_list *list, const char *value);

typedef void (*event_t)(uint64_t src, int argc, char **argv);

int     event_set(const char *event, event_t handler);
event_t event_get(const char *event);

#endif/*NATIO_H*/
