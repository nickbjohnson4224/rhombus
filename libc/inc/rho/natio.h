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

#include <rhombus.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <rho/path.h>
#include <rho/ipc.h>

/* core I/O *****************************************************************/

typedef uint64_t off_t;

size_t rp_read (rp_t rp, void *buf, size_t size, off_t offset);
size_t rp_write(rp_t rp, void *buf, size_t size, off_t offset);
int    rp_sync (rp_t rp);
int    rp_reset(rp_t rp);
int    rp_share(rp_t rp, void *buf, size_t size, off_t offset, int prot);

rp_t   rp_cons(rp_t rp, const char *type);
off_t  rp_size(rp_t rp);

/* filesystem operations ****************************************************/

extern rp_t fs_root;

rp_t  fs_find (const char *path);
rp_t  fs_lfind(const char *path);
rp_t  fs_cons (const char *path, const char *type);
char *fs_list (const char *path);
off_t fs_size (const char *path);

char *rp_list (rp_t dir);

/* file descriptor operations ***********************************************/

int open(const char *pathname, int flags);
int pipe(int pipefd[2]);

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

bool checktype(const char *path, const char *type);
bool checktype_rp(rp_t rp, const char *type);

/* access bitmap ************************************************************/

int getaccess(const char *path, uint32_t user);
int setaccess(const char *path, uint32_t user, int access);

#endif/*__RLIBC_NATIO_H*/
