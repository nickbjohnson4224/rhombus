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

#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdio.h>

extern FILE *fs_root;

#define FS_ERR  0x00
#define FS_FIND 0x01
#define FS_CONS 0x02
#define FS_MOVE 0x03
#define FS_REMV 0x04
#define FS_LINK 0x05
#define FS_LIST 0x06
#define FS_SIZE 0x07
#define FS_TYPE 0x08

struct fs_cmd {
	uint64_t v0;
	uint64_t v1;
	uint8_t  op;
	char     s0[4000];
	char     null0;
};

struct fs_cmd *fs_send(FILE *root, struct fs_cmd *cmd);

FILE    *fs_find  (FILE *root, const char *path);
FILE	*fs_cons  (FILE *dir, const char *name, int type);
FILE    *fs_move  (FILE *dir, const char *name, FILE *file);
char    *fs_list  (FILE *dir, int entry);
int      fs_remove(FILE *fobj);
int      fs_link  (FILE *link, FILE *fobj);
uint64_t fs_size  (FILE *file);
int      fs_type  (FILE *fobj);

void     fs_chroot(FILE *root);

#define FOBJ_NULL	0x00
#define FOBJ_FILE	0x01
#define FOBJ_DIR	0x02
#define FOBJ_LINK	0x03

#endif/*FS_H*/
