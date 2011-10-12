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

#ifndef __RLIBC_NATIO_H
#define __RLIBC_NATIO_H

#include <robject.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <path.h>
#include <ipc.h>

/* core I/O *****************************************************************/

typedef uint64_t off_t;

size_t read (rp_t rp, void *buf, size_t size, off_t offset);
size_t write(rp_t rp, void *buf, size_t size, off_t offset);
int    sync (rp_t rp);
int    reset(rp_t rp);
int    share(rp_t rp, void *buf, size_t size, off_t offset, int prot);

rp_t   rp_cons (rp_t rp, const char *type);
off_t  rp_size (rp_t rp);
void   rp_close(rp_t rp);
int    rp_open (rp_t rp);

/* filesystem operations ****************************************************/

extern rp_t fs_root;

rp_t  fs_find (const char *path);
rp_t  fs_lfind(const char *path);
rp_t  fs_cons (const char *path, const char *type);
char *fs_list (const char *path);
off_t fs_size (const char *path);

char *rp_list (rp_t dir);

/*****************************************************************************
 * links
 *
 * There are two different types of links: hard links and symbolic links.
 *
 * Hard links are simply directory entries, and are used for all directories
 * and files. Because of this, hard links are indistinguishable from files.
 * All metadata of a file is shared between its hard links, except for its
 * position in the VFS.
 *
 * Symbolic links are redirections to a specified path. Because paths may
 * contain resource pointers, symbolic links may be used like UNIX
 * mountpoints, because they can span devices without those devices having a
 * contiguous VFS. Symbolic links may be invalid as well, because they don't
 * keep track of their targets. Symbolic links may be created and modified
 * using fs_slink (and fs_plink, which is a more flexible form.)
 */

int fs_slink(const char *path, const char *link);
int fs_plink(const char *path, rp_t link_rp, const char *link_path);
int fs_link (const char *path, const char *link);
int fs_ulink(const char *path);

int rp_slink(rp_t rp, const char *link);
int rp_plink(rp_t rp, rp_t link_rp, const char *link_path);
int rp_link (rp_t dir, const char *name, rp_t link);

/* robject type system ******************************************************/

bool checktype   (const char *path, const char *type);
bool checktype_rp(rp_t rp, const char *type);

/*****************************************************************************
 * access bitmap
 *
 * The following flags correspond to bits that may be set in the access
 * bitmap. Permissions can be assigned on a per-user basis.
 *
 * ACCS_READ  - 0x2
 *
 * This flag allows read access. For directories and links, this means 
 * finding and listing. For files, this means reading file contents.
 *
 * ACCS_WRITE - 0x4
 *
 * This flag allows write access. For directories, this means the creation and
 * deletion of hard links. For files, this means writing, clearing, and 
 * deleting files/file contents, as well as requesting file synchronization.
 * 
 * ACCS_ALTER - 0x8
 *
 * This flag allows the access bitmap to be modified. Some drivers simply
 * do not allow certain operations (usually writing, if the filesystem is
 * read-only) and this does not ensure that the permission bitmap will 
 * actually be modified as specified.
 *
 * These three flags are guaranteed to be implemented by all drivers.
 */

#define ACCS_PING	0x01
#define ACCS_READ	0x02
#define ACCS_WRITE	0x04
#define ACCS_ALTER	0x08

uint8_t getaccess   (const char *path, uint32_t user);
uint8_t getaccess_rp(rp_t rp, uint32_t user);

int     setaccess   (const char *path, uint32_t user, uint8_t access);
int     setaccess_rp(rp_t rp, uint32_t user, uint8_t access);

#endif/*__RLIBC_NATIO_H*/
