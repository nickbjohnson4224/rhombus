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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ipc.h>

/* native I/O routines *****************************************************/

size_t ssend(uint64_t fd, void *r, void *s, size_t size, uint64_t off, uint8_t port);

size_t read (uint64_t fd, void *buf, size_t size, uint64_t offset);
size_t write(uint64_t fd, void *buf, size_t size, uint64_t offset);
int    sync (uint64_t fd);
int    reset(uint64_t fd);

/* filesystem operations ***************************************************/

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
#define FS_AUTH 0x0B

#define ERR_NULL 0x00
#define ERR_FILE 0x01
#define ERR_DENY 0x02
#define ERR_FUNC 0x03
#define ERR_TYPE 0x04

struct fs_cmd {
	uint64_t v0;
	uint64_t v1;
	uint8_t  op;
	char     s0[4000];
	char     null0;
};

struct fs_cmd *fs_send(uint64_t root, struct fs_cmd *cmd);

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

void     fs_chroot(FILE *root);

#define FOBJ_NULL	0x00
#define FOBJ_FILE	0x01
#define FOBJ_DIR	0x02

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
