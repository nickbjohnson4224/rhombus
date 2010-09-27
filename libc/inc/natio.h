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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* native I/O routines *****************************************************/

size_t ssend(FILE *file, void *r, void *s, size_t size, uint64_t off, uint8_t port);
size_t read (FILE *file, void *buf, size_t size, uint64_t offset);
size_t write(FILE *file, void *buf, size_t size, uint64_t offset);

/* native file operations **************************************************/

FILE *ffind(const char *path);

int fstat(FILE *stream, const char *field, const char *fmt, ...);
int fctrl(FILE *stream, const char *field, const char *fmt, ...);

/* virtual filesystem operations *******************************************/

extern FILE *vfs_root;

int vfs_get(const char *path, const char *field, const char *fmt, ...);
int vfs_set(const char *path, const char *field, const char *fmt, ...);
int vfs_lst(const char *path, char *buffer);

/* local filesystem operations (for drivers) *******************************/

int lfs_get(const char *path, const char *field, const char *fmt, ...);
int lfs_set(const char *path, const char *field, const char *fmt, ...);
int lfs_lnk(const char *path, const char *path1);
int lfs_add(const char *path, const char *name, uint32_t inode);
int lfs_mnt(const char *path, const char *name, FILE *target);

int         lfs_add_inode(uint32_t inode, const char *path);
const char *lfs_get_inode(uint32_t inode);

/* old VFS API *************************************************************/

int vflist (const char *path, char *buffer);
int vfdir  (const char *dir, const char *name);

int vfstat (const char *path, const char *field, const char *fmt, ...);
int vfctrl (const char *path, const char *field, const char *fmt, ...);
int vfstatl(const char *path, const char *field, const char *fmt, ...);
int vfctrll(const char *path, const char *field, const char *fmt, ...);

int vffile (const char *dir, const char *name, uint32_t inode);
int vfroot (FILE *target);
int vfmnt  (const char *dir, const char *name, FILE *target);
int vflmnt (const char *dir, const char *name, FILE *target);

#endif/*NATIO_H*/
