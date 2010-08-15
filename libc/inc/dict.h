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

#ifndef FLUX_DICT_H
#define FLUX_DICT_H

#include <stdint.h>
#include <stdbool.h>
#include <alloca.h>
#include <stdio.h>
#include <ipc.h>

/* dictionary interface ****************************************************/

struct __link {
	const char *pre;
	uint32_t server;
	uint64_t inode;
};

struct __link_req {
	char key[2048];
	char val[1024];
};

struct __dict {
	struct __dict *next[256];
	struct __link *link;
	char *value;
};

extern struct __info {
	struct __dict root;
	bool          mutex;
	uintptr_t     brk;
} *dict_info;

void dict_init(void);

/* data serialization ******************************************************/

#define tdeflate(d, s) (deflate(d, s, alloca(((s)*2)+2)))

char  *deflate(const void *data, size_t size, char *archivebuffer);
size_t inflate(void *databuffer, size_t size, const char *archive);

/* read functions **********************************************************/

char *dread  (const char *key);
char *dreadns(const char *ns, const char *key);
char *dreadr (FILE *remote, const char *key);

/* write functions *********************************************************/

int dwrite   (const char *value, const char *key);
int dwritens (const char *value, const char *ns, const char *key);

/* link functions **********************************************************/

int dlink    (const char *key, const char *pre, FILE *target);

/* internal functions ******************************************************/

char *_dlink_read (struct __link *l, const char *key);
int   _dlink_write(struct __link *l, const char *val, const char *key);
int   _dlink_link (struct __link *l, struct __link *new, const char *key);

/* dictionary event handlers ***********************************************/

void _devent_read (uint32_t caller, struct packet *packet);
void _devent_write(uint32_t caller, struct packet *packet);
void _devent_link (uint32_t caller, struct packet *packet);

/* dictionary heap - persistent ********************************************/

void *dalloc(size_t size);

#endif/*FLUX_DICT_H*/
