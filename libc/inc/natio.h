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

extern FILE *stdvfs;

size_t ssend(FILE *file, void *r, void *s, size_t size, uint64_t off, uint8_t port);
size_t read (FILE *file, void *buf, size_t size, uint64_t offset);
size_t write(FILE *file, void *buf, size_t size, uint64_t offset);
size_t query(FILE *file, void *rbuf, void *sbuf, size_t size);

/* VFS operations **********************************************************/

#define VFS_CMD_FIND  0
#define VFS_CMD_ADD   1
#define VFS_CMD_LIST  2
#define VFS_CMD_LINK  3

#define VFS_CMD_REPLY 10
#define VFS_CMD_ERROR 11

struct vfs_query {
	uint32_t command;
	uint32_t server;
	uint64_t inode;
	uint8_t  naddr[16];
	char path0[1000];
	char path1[1000];
};

int find (const char *path, uint32_t *server, uint64_t *inode);
int fadd (const char *path, uint32_t server, uint64_t inode);
int list (const char *path, char *buffer);

/* info system (obsolete due to dictionary) ********************************/

struct info_query {
	char field[100];
	char value[1000];
};

bool   info (FILE *file, char *value, const char *field);
bool   ctrl (FILE *file, char *value, const char *field);

#endif/*NATIO_H*/
